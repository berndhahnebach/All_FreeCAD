#***************************************************************************
#*                                                                         *
#*   Copyright (c) 2011                                                    *  
#*   Yorik van Havre, Marijn van Aerle                                     *  
#*                                                                         *
#*   This program is free software; you can redistribute it and/or modify  *
#*   it under the terms of the GNU General Public License (GPL)            *
#*   as published by the Free Software Foundation; either version 2 of     *
#*   the License, or (at your option) any later version.                   *
#*   for detail see the LICENCE text file.                                 *
#*                                                                         *
#*   This program is distributed in the hope that it will be useful,       *
#*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
#*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
#*   GNU Library General Public License for more details.                  *
#*                                                                         *
#*   You should have received a copy of the GNU Library General Public     *
#*   License along with this program; if not, write to the Free Software   *
#*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
#*   USA                                                                   *
#*                                                                         *
#***************************************************************************

import os,re, copy

__title__="FreeCAD IFC parser"
__author__ = "Yorik van Havre, Marijn van Aerle"
__url__ = "http://free-cad.sourceforge.net"

'''
FreeCAD IFC parser, by Yorik van Havre, based on work by Marijn van Aerle

Usage:
        import ifcReader
        ifcdoc = ifcReader.IfcDocument("path/to/file.ifc")
        print ifcdoc.Entities
        myent = ifcdoc.Entities[20] # returns one entity
        myent = ifcdoc.getEnt(20) # alternative way
        polylines = ifcdoc.getEnt("IFCPOLYLINE") # returns a list
        print myent.attributes

The ifc document contains a list of entities, that can be retrieved
by iterating the list (indices corresponds to the entities ids)
or by using the getEnt() method. All entities have id, type
and attributes. Attributes can have values such as text or number,
or a link to another entity.

Important note:

For this reader to function, you need an IFC Schema Express file (.exp)
available here:
http://www.steptools.com/support/stdev_docs/express/ifc2x3/ifc2x3_tc1.exp
For licensing reasons we are not allowed to ship that file with FreeCAD.
Just place the .exp file together with this script.
'''

IFCLINE_RE = re.compile("#(\d+)[ ]?=[ ]?(.*?)\((.*)\);[\\r]?$")
DEBUG = False

class IfcSchema:
    SIMPLETYPES = ["INTEGER", "REAL", "STRING", "NUMBER", "LOGICAL", "BOOLEAN"]
    NO_ATTR = ["WHERE", "INVERSE","WR2","WR3", "WR4", "WR5", "UNIQUE", "DERIVE"]

    def __init__(self, filename):
        self.filename = filename
        if not os.path.exists(filename):
            raise ImportError("no IFCSchema file found!")
        else:
            self.file = open(self.filename)
            self.data = self.file.read()
            self.types = self.readTypes()
            self.entities = self.readEntities()
            if DEBUG: print "Parsed from schema %s: %s entities and %s types" % (self.filename, len(self.entities), len(self.types))

    def readTypes(self):
        """
        Parse all the possible types from the schema, 
        returns a dictionary Name -> Type
        """
        types = {}
        for m in re.finditer("TYPE (.*) = (.*);", self.data):
            typename, typetype = m.groups() 
            if typetype in self.SIMPLETYPES:
                types[typename] = typetype
            else:
                types[typename] = "#" + typetype
                
        return types
        
    def readEntities(self):
        """
        Parse all the possible entities from the schema,
        returns a dictionary of the form:
        { name: { 
            "supertype": supertype, 
            "attributes": [{ key: value }, ..]
        }}  
        """
        entities = {}
        
        # Regexes must be greedy to prevent matching outer entity and end_entity strings
        # Regexes have re.DOTALL to match newlines
        for m in re.finditer("ENTITY (.*?)END_ENTITY;", self.data, re.DOTALL):
            entity = {}
            raw_entity_str = m.groups()[0]

            entity["name"] = re.search("(.*?)[;|\s]", raw_entity_str).groups()[0].upper()

            subtypeofmatch = re.search(".*SUBTYPE OF \((.*?)\);", raw_entity_str)
            entity["supertype"] = subtypeofmatch.groups()[0].upper() if subtypeofmatch else None

            # find the shortest string matched from the end of the entity type header to the
            # first occurence of a NO_ATTR string (when it occurs on a new line)
            inner_str = re.search(";(.*?)$", raw_entity_str, re.DOTALL).groups()[0]            

            attrs_str = min([inner_str.partition("\r\n "+a)[0] for a in self.NO_ATTR])
            attrs = []
            for am in re.finditer("(.*?) : (.*?);", attrs_str, re.DOTALL):
                name, attr_type = [s.replace("\r\n\t","") for s in am.groups()]
                attrs.append((name, attr_type))
            
            entity["attributes"] = attrs
            entities[entity["name"]] = entity
        

        return entities

    def getAttributes(self, name):
        """
        Get all attributes af an entity, including supertypes
        """
        ent = self.entities[name]

        attrs = []
        while ent != None:
            this_ent_attrs = copy.copy(ent["attributes"])
            this_ent_attrs.reverse()
            attrs.extend(this_ent_attrs)
            ent = self.entities.get(ent["supertype"], None)

        attrs.reverse()
        return attrs

class IfcFile:
    """
    Parses an ifc file given by filename, entities can be retrieved by name and id
    The whole file is stored in a dictionary (in memory)
    """
    
    entsById = {}
    entsByName = {}

    def __init__(self, filename,schema):
        self.filename = filename
        self.schema = IfcSchema(schema)
        self.file = open(self.filename)
        self.entById, self.entsByName, self.header = self.read()
        self.file.close()
        if DEBUG: print "Parsed from file %s: %s entities" % (self.filename, len(self.entById))
    
    def getEntityById(self, id):
        return self.entById.get(id, None)
    
    def getEntitiesByName(self, name):
        return self.entsByName.get(name, None)

    def read(self):
        """
        Returns 2 dictionaries, entById and entsByName
        """
        entById = {}
        entsByName = {}
        header = 'HEADER '
        readheader = False
        for line in self.file:
            e = self.parseLine(line)
            if e:
                entById[int(e["id"])] = e
                ids = e.get(e["name"],[])
                ids.append(e["id"])
                entsByName[e["name"]] = list(set(ids))
            elif 'HEADER' in line:
                readheader = True
            elif readheader:
                if 'ENDSEC' in line:
                    readheader = False
                else:
                    header += line
                    
        return [entById, entsByName, header]

    def parseLine(self, line):
        """
        Parse a line 
        """ 
        m = IFCLINE_RE.search(line)  # id,name,attrs
        if m:
            id, name, attrs = m.groups()
        else:
            return False
        
        return {"id": id, "name": name, "attributes": self.parseAttributes(name, attrs)}

    def parseAttributes(self, ent_name, attrs_str):
        """
        Parse the attributes of a line
        """
        parts = []
        lastpos = 0
        
        while lastpos < len(attrs_str):
            newpos = self.nextString(attrs_str, lastpos)
            parts.extend(self.parseAttribute(attrs_str[lastpos:newpos-1]))
            lastpos = newpos
        
        schema_attributes = self.schema.getAttributes(ent_name)

        assert len(schema_attributes) == len(parts), \
            "Expected %s attributes, got %s (entity: %s" % \
            (len(schema_attributes), len(parts), ent_name)
        
        attribute_names = [a[0] for a in schema_attributes]
        
        return dict(zip(attribute_names, parts))

    def parseAttribute(self, attr_str):
        """
        Map a single attribute to a python type (recursively)
        """
        parts = []
        lastpos = 0
        while lastpos < len(attr_str):
            newpos = self.nextString(attr_str, lastpos)
            s = attr_str[lastpos:newpos-1]
            if (s[0] == "(" and s[-1] == ")"): # list, recurse
                parts.append(self.parseAttribute(s[1:-1]))
            else:
                try:
                    parts.append(float(s)) # number, any kind
                except ValueError:
                    if s[0] == "'" and s[-1] == "'": # string
                        parts.append(s[1:-1])
                    elif s == "$":
                        parts.append(None)
                    else:
                        parts.append(s) # ref, enum or other

            lastpos = newpos
        
        return parts


    def nextString(self, s, start):
        """
        Parse the data part of a line
        """
        parens = 0
        quotes = 0

        for pos in range(start,len(s)):
            c = s[pos]
            if c == "," and parens == 0 and quotes == 0:
                return pos+1
            elif c == "(" and quotes == 0:
                parens += 1
            elif c == ")" and quotes == 0:
                parens -= 1
            elif c == "\'" and quotes == 0:
                quotes = 1
            elif c =="\'" and quotes == 1:
                quotes = 0
            
        return len(s)+1                  

class IfcEntity:
    "a container for an IFC entity"
    def __init__(self,ent):
        self.data = ent
        self.id = int(ent['id'])
        self.type = ent['name'].upper().strip(",[]()")
        self.attributes = ent['attributes']

    def __repr__(self):
        return str(self.id) + ' : ' + self.type + ' ' + str(self.attributes)
        
class IfcDocument:
    "an object representing an IFC document"
    def __init__(self,filename,schema="IFC2X3_TC1.exp"):
        f = IfcFile(filename,schema)
        self.filename = filename
        self.data = f.entById
        self.Entities = [f.header]
        for k,e in self.data.iteritems():
            self.Entities.append(IfcEntity(e))
        if DEBUG: print len(self.Entities)," entities created. Creating attributes..."
        for ent in self.Entities:
            if hasattr(ent,"attributes"):
                #ent.attributes = IfcAttribute(ent.attributes,self)
                for k,v in ent.attributes.iteritems():
                    if DEBUG: print "parsing attribute: ",k," value ",v
                    if isinstance(v,str):
                        val = v.strip()
                        if '#' in val:
                            if not "," in val:
                                val = val.strip()
                                val = val.replace("#","")
                                val = val.strip(" ()")
                                if DEBUG: print "referencing ",val," : ",self.getEnt(int(val))
                                val =  self.getEnt(int(val))
                    else:
                        val = v
                    setattr(ent,k.strip(),val)
        if DEBUG: print "Document successfully created"
                
    def __repr__(self):
        return "IFC Document: " + self.filename + ', ' + str(len(self.Entities)) + " entities"

    def getEnt(self,ref):
        "gets an entity by id number, or a list of entities by type"
        if isinstance(ref,int):
            return self.Entities[ref]
        elif isinstance(ref,str):
            l = []
            ref = ref.upper()
            for ob in self.Entities:
                if hasattr(ob,"type"):
                    if ob.type == ref:
                        l.append(ob)
            return l

    def search(self,pat):
        "searches entities types for partial match"
        l = []
        pat = pat.upper()
        for ob in self.Entities:
            if hasattr(ob,"type"):
                if pat in ob.type:
                    if not ob.type in l:
                        l.append(ob.type)
        return l
                        
if __name__ == "__main__":
    print __doc__

