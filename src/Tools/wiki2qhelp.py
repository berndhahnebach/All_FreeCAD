#!/usr/bin/env python

#***************************************************************************
#*                                                                         *
#*   Copyright (c) 2009 Yorik van Havre <yorik@gmx.fr>                     *  
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

__title__="wiki2qhelp"
__author__ = "Yorik van Havre <yorik@gmx.fr>"
__url__ = "http://yorik.orgfree.com"

"""
This script retrieves the contents of a wiki site and saves it locally,
then calls qt help compiler to produce a qhelp-assistant help file.
The script can be called without arguments, it will then use the default
url below, or by passing it an url and optionally a TOC name.
"""

import sys, os, re, tempfile
from urllib2 import urlopen, HTTPError

#    CONFIGURATION       #################################################

DEFAULTURL = "http://sourceforge.net/apps/mediawiki/free-cad" #default URL if no URL is passed
INDEX = "Online_Help_Toc" # the start page from where to crawl the wiki
NORETRIEVE = ['Manual','Developer_hub','Power_users_hub','Users_hub','Source_documentation', 'User_hub','Main_Page','About_this_site'] # pages that won't be fetched (kept online)
MAXFAIL = 3 # max number of retries if download fails
VERBOSE = True # to display what's going on. Otherwise, runs totally silent.
COMPILE = True # Wether qt assistant will be used to compile the final help file
OUTPUTPATH = os.path.expanduser("~")+os.sep+'.FreeCAD' # Where to store the qch file

#    END CONFIGURATION      ##############################################

URL = DEFAULTURL
TMPFOLDER = tempfile.mkdtemp()
wikiindex = "/index.php?title="
processed = []
usage='''
    wiki2qhelp [options] [url] [index page] [output path]

    fetches wiki pages from the specified url, starting from specified
    index page, and outputs a .qch file in the specified output path.
    You must have qassistant installed.

    If no url, index page or output path is specified, the following
    default values will be used:
    url: '''+DEFAULTURL+'''
    index page: '''+INDEX+'''
    output path: '''+OUTPUTPATH+'''

    Options:

    -h or --help: Displays this help message
    '''
css = """/* Basic CSS for offline wiki rendering */

body {
  font-family: Arial,Helvetica,sans-serif;
  font-size: 13px;
  text-align: justify;
  }

h1 {
  font-size: 2.2em;
  font-weight: bold;
  background: #46A4D0;
  color: white;
  padding: 5px;
  -moz-border-radius: 5px;
  -webkit-border-radius: 5px;
  }

pre {
  border: 1px dashed #333333;
  text-align: left;
  background: #EEEEEE;
  padding: 5px;
  }

a:link, a:visited {
  font-weight: bold;
  text-decoration: none;
  color: #0084FF;
  }

a:hover {
  text-decoration: underline;
  }

.printfooter {
  font-size: 0.8em;
  color: #333333;
  border-top: 1px solid #333333;
  }

.wikitable #toc {
  font-size: 0.8em;
  }

#toc {
  display: none;
  }

"""

def crawl(site=DEFAULTURL):
    "downloads an entire wiki site"
    if COMPILE and os.system('qhelpgenerator -v'):
        print "Error: Qassistant not fully installed, exiting."
        return 1
    URL = site
    if VERBOSE: print "crawling ", URL, ", saving in ", TMPFOLDER
    if not os.path.isdir(TMPFOLDER): os.mkdir(TMPFOLDER)
    file = open(TMPFOLDER + os.sep + "wiki.css",'wb')
    file.write(css)
    file.close()
    todolist = []
    count = 1
    indexpages = get(INDEX)
    todolist.extend(indexpages)
    while todolist:
        targetpage = todolist.pop()
        if not targetpage in NORETRIEVE:
            if VERBOSE: print count, ": Fetching ", targetpage
            pages = get(targetpage)
            count += 1
            processed.append(targetpage)
            for p in pages:
                if (not (p in todolist)) and (not (p in processed)):
                    todolist.append(p)
    if VERBOSE: print "Fetched ", count, " pages"
    qhp = buildtoc()
    if COMPILE:
        if compile(qhp):
            print "Temp Folder ",TMPFOLDER," has not been deleted."
            return 1
        else:
            if VERBOSE: print "Deleting temp files..."
            os.rmdir(TMPFOLDER)
    if VERBOSE: print "All done!"
    return 0

def compile(qhpfile):
    "compiles the whole html doc with qassistant"
    qchfile = OUTPUTPATH + os.sep + "freecad.qch"
    if not os.system('qhelpgenerator '+qhpfile+' -o '+qchfile):
        if VERBOSE: print "Successfully created ",qchfile
        return 0

def buildtoc(folder=TMPFOLDER,page=INDEX):
    "gets the table of contents page and parses its contents into a clean lists structure"
    
    qhelpfile = '''<?xml version="1.0" encoding="UTF-8"?>
    <QtHelpProject version="1.0">
        <namespace>FreeCAD</namespace>
        <virtualFolder>doc</virtualFolder>
        <customFilter name="FreeCAD 0.9">
            <filterAttribute>FreeCAD</filterAttribute>
            <filterAttribute>0.9</filterAttribute>
        </customFilter>
        <filterSection>
            <filterAttribute>FreeCAD</filterAttribute>
            <filterAttribute>0.9</filterAttribute>
            <toc>
                <inserttoc>
            </toc>
            <keywords>
                <insertkeywords>
            </keywords>
            <insertfiles>
        </filterSection>
    </QtHelpProject>
    '''
    
    def getname(line):
        line = re.compile('<li>').sub('',line)
        line = re.compile('</li>').sub('',line)
        title = line.strip()
        link = ''
        if "<a" in line:
            title = re.findall('<a[^>]*>(.*?)</a>',line)[0].strip()
            link = re.findall('href="(.*?)"',line)[0].strip()
        return title,link

    if VERBOSE: print "Building table of contents..."
    f = open(folder+os.sep+page+'.html')
    html = ''
    for line in f: html += line
    f.close()
    html = html.replace("\n"," ")
    html = html.replace("> <","><")
    html = re.findall("<ul.*/ul>",html)[0]
    items = re.findall('<li[^>]*>.*?</li>|</ul></li>',html)
    inserttoc = '<section> title="Table of Contents"\n'
    insertkeywords = ''
    for item in items:
        if not ("<ul>" in item):
            if ("</ul>" in item):
                inserttoc += '</section>\n'
            else:
                link = ''
                title,link=getname(item)
                if link:
                    link='" ref="'+link
                    insertkeywords += ('<keyword name="'+title+link+'"/>\n')
                inserttoc += ('<section> title="'+title+link+'"</section>\n')
        else:
            subitems = item.split("<ul>")
            for i in range(len(subitems)):
                link = ''
                title,link=getname(subitems[i])
                if link:
                    link='" ref="'+link
                    insertkeywords += ('<keyword name="'+title+link+'"/>\n')
                trail = ''
                if i == len(subitems)-1: trail = '</section>'
                inserttoc += ('<section> title="'+title+link+'"'+trail+'\n')
    inserttoc += '</section>\n'

    insertfiles = "<files>\n"
    for fil in os.listdir(folder):
        insertfiles += ("<file>"+fil+"</file>\n")
    insertfiles += "</files>\n"

    qhelpfile = re.compile('<insertkeywords>').sub(insertkeywords,qhelpfile)
    qhelpfile = re.compile('<inserttoc>').sub(inserttoc,qhelpfile)
    qhelpfile = re.compile('<insertfiles>').sub(insertfiles,qhelpfile)
    qfilename = folder + os.sep + "freecad.qhp"
    f = open(qfilename,'wb')
    f.write(qhelpfile)
    f.close()
    if VERBOSE: print "Done writing qch file."
    return qfilename

def get(page):
    "downloads a single page, returns the other pages it links to"
    html = fetchpage(page)
    html = cleanhtml(html)
    pages = getlinks(html)
    html = cleanlinks(html,pages)
    html = cleanimagelinks(html)
    output(html,page)
    return pages

def cleanhtml(html):
    "cleans given html code from dirty script stuff"
    html = html.replace('\n','Wlinebreak') # removing linebreaks for regex processing
    html = re.compile('(.*)<div[^>]+column-content+[^>]+>').sub('',html) # stripping before content
    html = re.compile('<div[^>]+column-one+[^>]+>.*').sub('',html) # stripping after content
    html = re.compile('<!--[^>]+-->').sub('',html) # removing comment tags
    html = re.compile('<script[^>]*>.*?</script>').sub('',html) # removing script tags
    html = re.compile('<!--\[if[^>]*>.*?endif\]-->').sub('',html) # removing IE tags
    html = re.compile('<div id="jump-to-nav"[^>]*>.*?</div>').sub('',html) # removing nav div
    html = re.compile('<h3 id="siteSub"[^>]*>.*?</h3>').sub('',html) # removing print subtitle
    html = re.compile('Retrieved from').sub('Online version:',html) # changing online title
    html = re.compile('<div id="mw-normal-catlinks[^>]>.*?</div>').sub('',html) # removing catlinks
    html = re.compile('<div class="NavHead.*?</div>').sub('',html) # removing nav stuff
    html = re.compile('<div class="NavContent.*?</div>').sub('',html) # removing nav stuff
    html = re.compile('<div class="NavEnd.*?</div>').sub('',html) # removing nav stuff
    html = re.compile('Wlinebreak').sub('\n',html) # restoring original linebreaks
    return html
    
def getlinks(html):
    "returns a list of wikipage links in html file"
    links = re.findall('<a[^>]*>.*?</a>',html)
    pages = []
    for l in links:
        # rg = re.findall('php\?title=(.*)\" title',l)
        rg = re.findall('href=.*?php\?title=(.*?)"',l)
        if rg:
            rg = rg[0]
            if "#" in rg:
                rg = rg.split('#')[0]
            if ":" in rg:
                NORETRIEVE.append(rg)
            if ";" in rg:
                NORETRIEVE.append(rg)
            if "&" in rg:
                NORETRIEVE.append(rg)
            pages.append(rg)
    return pages

def getimagelinks(html):
    "returns a list of image links found in an html file"
    return re.findall('<img.*?src="(.*?)"',html)

def cleanlinks(html, pages=None):
    "cleans page links found in html"
    if not pages: pages = getlinks(html)
    for page in pages:
        if  page in NORETRIEVE:
            output = 'href="' + URL + wikiindex + page + '"'
        else:
            output = 'href="' + page + '.html"'
        html = re.compile('href="[^"]+' + page + '"').sub(output,html)
    return html

def cleanimagelinks(html,links=None):
    "cleans image links in given html"
    if not links: links = getimagelinks(html)
    if links:
        for l in links:
            nl = re.findall('.*/(.*)',l)
            if nl: html = html.replace(l,nl[0])
            fetchimage(l)
    return html

def fetchpage(page):
    "retrieves given page from the wiki"
    failcount = 0
    while failcount < MAXFAIL:
        try:
            html = (urlopen(URL + wikiindex + page).read())
            return html
        except HTTPError:
            failcount += 1
    print 'Error: unable to fetch page ' + page

def fetchimage(imagelink):
    "retrieves given image from the wiki and saves it"
    filename = re.findall('.*/(.*)',imagelink)[0]
    if not (filename in processed):
        failcount = 0
        while failcount < MAXFAIL:
            try:
                if VERBOSE: print "Fetching " + filename
                data = (urlopen(webroot(URL) + imagelink).read())
                path = local(filename,image=True)
                file = open(path,'wb')
                file.write(data)
                file.close()
                processed.append(filename)
            except HTTPError:
                failcount += 1
        print 'Error: unable to fetch file ' + filename

def local(page,image=False):
    "returns a local path for a given page/image"
    if image:
        return TMPFOLDER + os.sep + page
    else:
        return TMPFOLDER + os.sep + page + '.html'

def exists(page,image=False):
    "checks if given page/image already exists"
    path = local(page,image)
    if os.path.exists(path): return True
    return False

def webroot(url):
    return re.findall('(http://.*?)/',url)[0]

def output(html,page):
    "encapsulates raw html code into nice html body"
    header = "<html><head>"
    header += "<title>"
    header += page
    header += "</title>"
    header += "<link type='text/css' href='wiki.css' rel='stylesheet'>"
    header += "</head><body>"
    footer = "</body></html>"
    html = header+html+footer
    filename = local(page)
    file = open(filename,'wb')
    file.write(html)
    file.close()

def main(arg):
    if arg:
        if (arg[0] == '-h') or (arg[0] == '--help'):
            print usage
        else:
            URL = arg[0]
            if len(arg) > 1: INDEX = arg[1]
            if len(arg) > 2: OUTPUTPATH = arg[2]
            crawl()
    else:
        crawl()
    
if __name__ == "__main__":
	main(sys.argv[1:])
      
