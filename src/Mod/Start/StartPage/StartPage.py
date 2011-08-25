import os,FreeCAD,FreeCADGui,tempfile,time,zipfile
from PyQt4 import QtGui

def translate(context,text):
        "convenience function for the Qt translator"
        return str(QtGui.QApplication.translate(context, text, None, QtGui.QApplication.UnicodeUTF8).toUtf8())

# texts to be translated

text01 = translate("StartPage","FreeCAD Start Center")
text02 = translate("StartPage","Start a new project")
text03 = translate("StartPage","Recent Files")
text04 = translate("StartPage","Demos")
text05 = translate("StartPage","Tutorials")
text06 = translate("StartPage","Homepage")
text07 = translate("StartPage","This is the FreeCAD Homepage. Here you will be able to find a lot of information about FreeCAD, tutorials, examples and user documentation.")
text08 = translate("StartPage","FreeCAD Homepage")
text09 = translate("StartPage","Example projects")
text10 = translate("StartPage","Schenkel STEP file")
text11 = translate("StartPage","Load a PartDesign example")
text12 = translate("StartPage","Load a Drawing extraction")
text13 = translate("StartPage","Load a Robot simulation example")
text14 = translate("StartPage","Projects from the Web")
text15 = translate("StartPage","Schenkel STEP")
text16 = translate("StartPage","Complex Part")
text17 = translate("StartPage","Close this window after opening or creating a file")
text18 = translate("StartPage","Don't show me this window again next time")
text19 = translate("StartPage","Designing parts")
text20 = translate("StartPage","The <b>Part Design</b> workbench is designed to create complex pieces based on constrained 2D sketches. Use it to draw 2D shapes, constrain some of their elements and extrude them to form 3D pieces.")
text21 = translate("StartPage","Example workflow")
text22 = translate("StartPage","Part Design")
text23 = translate("StartPage","Designing architectural elements")
text24 = translate("StartPage","The <b>Architectural Design</b> workbench is specially designed for working with architectural elements such as walls or windows. Start by drawing 2D shapes, and use them as guides to build architecutral objects.")
text25 = translate("StartPage","Architectual Design")
text26 = translate("StartPage","Working with Meshes")
text27 = translate("StartPage","The <b>Mesh Workbench</b> is used to work with Mesh objects. Meshes are simpler 3D objects than Part objects, but they are often easier to import and export to/from other applications.")
text28 = translate("StartPage","FreeCAD offers you several tools to convert between Mesh and Part objects.")
text29 = translate("StartPage","Work with Meshes")
text30 = translate("StartPage","The complete workbench")
text31 = translate("StartPage","FreeCAD default workbench")
text32 = translate("StartPage","populated with some of the most commonly used tools.")
text33 = translate("StartPage","file size:")
text34 = translate("StartPage","creation time:")
text35 = translate("StartPage","last modified:")

# here is the html page skeleton

page = """
<html>
  <head>
    <title>FreeCAD - Start page</title>
    <script language="javascript">
      function show(theText) {
        ddiv = document.getElementById("description");
        if (theText == "") theText = "&nbsp;";
        ddiv.innerHTML = theText;
      }
    </script>
    <style type="text/css">
      body {
        background: #171A2B url(Background.jpg);
        color: white;
        font-family: Arial, Helvetica, Sans;
        font-size: 11px;
      }
      a {
        color: #0092E8;
        font-weight: bold;
        text-decoration: none;
        padding: 2px;
      }
      a:hover {
        color: white;
        background: #0092E8;
        border-radius: 5px;
      }
      p {
        text-align: justify;
      }
      h1 {
        font-size: 3em;
        letter-spacing: 2px;
        padding: 20px 0 0 80px;
        align: bottom;
      }
      h2 {
        font-size: 1.2em;
      }
      ul {
        list-style-type: none;
        padding: 0;
      }
      .column {
        width: 300px;
        float: left;
        margin-left: 10px;
      }
      .block {
        background: rgba(30,31,33,0.6);;
        border-radius: 5px;
        padding: 8px;
        margin-bottom: 10px;
      }
      .options {
        clear: both;
      }
    </style>
  </head>

  <body>

    <h1><img src="FreeCAD.png">&nbsp;""" + text01 + """</h1>

    <div class="column">

      <div class="block">
        <h2>""" + text02 + """</h2>
          defaultworkbenches
      </div>

      <div class="block">
        <h2>""" + text03 + """</h2>
          recentfiles
      </div>

      <div class="block">
        <h2>""" + text04 + """</h2>
      </div>

      <div class="block">
        <h2>""" + text05 + """</h2>
      </div>

    </div>

    <div class="column">

      <div class="block">
        <h2>""" + text06 + """</h2>
        <ul>
          <li><img src="web.png">&nbsp;
              <a onMouseover="show('<p>""" + text07 + """</p>')" 
                 onMouseout="show('')"
                 href="http://free-cad.sf.net/">""" + text08 + """</a></li>
        </ul>
      </div>

      <div class="block">
        <h2>""" + text09 + """</h2>
        <ul>
          <li><a href="LoadSchenkel.py">""" + text10 + """</a></li>
          <li><a href="LoadPartDesignExample.py">""" + text11 + """</a></li>
          <li><a href="LoadDrawingExample.py">""" + text12 + """</a></li>
          <li><a href="LoadRobotExample.py">""" + text13 + """</a></li>
        </ul>
      </div>

      <div class="block">
        <h2>""" + text14 + """</h2>
        <ul>
          <li><a href="http://freecad-project.de/svn/ExampleData/FileFormates/Schenkel.stp">""" + text15 + """</a></li>
          <li><a href="http://freecad-project.de/svn/ExampleData/Examples/CAD/Complex.FCStd">""" + text16 + """</a></li>
        </ul>
      </div>

    </div>

    <div class="column" id="description">
      &nbsp;
    </div>

    <form class="options">
      <input type="checkbox" name="closeThisDialog">
      """ + text17 + """<br/>
      <input type="checkbox" name="dontShowAgain">
      """ + text18 + """
    </form>

  </body>
</html>
"""

def getWorkbenches():
        return """
        <ul>    
          <li><img src="PartDesign.png">&nbsp;
              <a onMouseover="show('<h3>""" + text19 + """</h3> \
                              <p>""" + text20 + """</p><p><small>""" + text21 + """ \
                 :</small></p><img src=PartDesignExample.png>')" 
                 onMouseout="show('')" 
                 href="PartDesign.py">""" + text22 + """</a></li>
          <li><img src="ArchDesign.png">&nbsp;
              <a onMouseover="show('<h3>""" + text23 + """</h3> \
                              <p>""" + text24 + """</p>')" 
                 onMouseout="show('')"
                 href="ArchDesign.py">""" + text25 + """</a></li>
          <li><img src="Mesh.png">&nbsp;
              <a onMouseover="show('<h3>""" + text26 + """</h3> \
                              <p>""" + text27 + """</p><p>""" + text28 + """</p>')" 
                 onMouseout="show('')" 
                 href="Mesh.py">""" + text29 + """</a></li>
          <li><img src="Complete.png">&nbsp;
              <a onMouseover="show('<h3>""" + text30 +"""</h3> \
                              <p>This is the <b>""" + text31 + """</b>, \
                              """ + text32 + """</p>')" 
                 onMouseout="show('')" 
                 href="DefaultWorkbench.py">""" + text31 + """</a></li>
        </ul>
"""

def getInfo(filename):
        "returns available file information"

        def getLocalTime(timestamp):
                "returns a local time from a timestamp"
                
                return time.strftime("%m/%d/%Y %H:%M:%S",time.localtime(timestamp))

        def getSize(size):
                "returns a human-readable size"
                
                if size > 1024*1024:
                        hsize = str(size/(1024*1024)) + "Mb"
                elif size > 1024:
                        hsize = str(size/1024) + "Kb"
                else:
                        hsize = str(size) + "b"
                return hsize
        
        html = '<h3>'+os.path.basename(filename)+'</h3>'
        
        if os.path.exists(filename):
                # get normal file info
                s = os.stat(filename)
                html += "<p>" + text33 + " " + getSize(s.st_size) + "<br/>"
                html += text34 + " " + getLocalTime(s.st_ctime) + "<br/>"
                html += text35 + " " + getLocalTime(s.st_mtime) + "</p>"
                # get additional info from fcstd files
                if os.path.splitext(filename)[1] in [".fcstd",".FcStd"]:
                        zfile=zipfile.ZipFile(filename)
                        files=zfile.namelist()
                        # check for meta-file if it's really a FreeCAD document
                        if files[0] == "Document.xml":
                                html += "<p>FreeCAD Standard File</p>"
                                image="thumbnails/Thumbnail.png"
                                if image in files:
                                        image=zfile.read(image)
                                        thumbfile = tempfile.mkstemp(suffix='.png')[1]
                                        thumb = open(thumbfile,"wb")
                                        thumb.write(image)
                                        thumb.close()
                                        html += '<img src=file://'

                                        html += thumbfile + '><br/>'
        else:
                html += "<p>File not found</p>"
                
        return html

def getRecentFiles():
        "returns a list of 3 latest recent files"
        
        rf=FreeCAD.ParamGet("User parameter:BaseApp/Preferences/RecentFiles")
        ct=rf.GetInt("RecentFiles")
        html = '<ul>'
        for i in range(3):
                if i < ct:
                        mr = rf.GetString("MRU%d" % (i))
                        fn = os.path.basename(mr)
                        html += '<li><a '
                        html += 'onMouseover="show(\''+getInfo(mr)+'\')" '
                        html += 'onMouseout="show(\'\')" '
                        html += 'href="LoadMRU'+str(i)+'.py">'
                        html += fn
                        html += '</a></li>'
        html += '</ul>'
        return html

def handle():
        "returns the complete html startpage"
        
        # add recent files
        recentfiles = getRecentFiles()
        html = page.replace("recentfiles",recentfiles)

        # add default workbenches
        html = html.replace("defaultworkbenches",getWorkbenches())
        
        return html
	
