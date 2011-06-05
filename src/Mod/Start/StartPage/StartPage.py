import os,FreeCAD,FreeCADGui,tempfile,time,zipfile

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
        background: #171A2B url(basepath/Background.jpg);
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

    <h1><img src="basepath/FreeCAD.png">&nbsp;FreeCAD Start Center</h1>

    <div class="column">

      <div class="block">
        <h2>Start a new project</h2>
          defaultworkbenches
      </div>

      <div class="block">
        <h2>Recent Files</h2>
          recentfiles
      </div>

      <div class="block">
        <h2>Demos</h2>
      </div>

      <div class="block">
        <h2>Tutorials</h2>
      </div>

    </div>

    <div class="column">

      <div class="block">
        <h2>Homepage</h2>
        <ul>
          <li><img src="basepath/web.png">&nbsp;
              <a onMouseover="show('<p>This is the FreeCAD Homepage. Here you will be \
                              able to find a lot of information about FreeCAD, tutorials, \
                              examples and user documentation.</p>')" 
                 onMouseout="show('')"
                 href="http://free-cad.sf.net/">FreeCAD Homepage</a></li>
        </ul>
      </div>

      <div class="block">
        <h2>Example projects</h2>
        <ul>
          <li><a href="basepath/LoadSchenkel.py">Schenkel STEP file</a></li>
          <li><a href="basepath/LoadPartDesignExample.py">Load a PartDesign example</a></li>
          <li><a href="basepath/LoadDrawingExample.py">Load a Drawing extraction</a></li>
          <li><a href="basepath/LoadRobotExample.py">Load a Robot simulation example</a></li>
        </ul>
      </div>

      <div class="block">
        <h2>Projects from the Web</h2>
        <ul>
          <li><a href="http://freecad-project.de/svn/ExampleData/FileFormates/Schenkel.stp">Schenkel STEP</a></li>
          <li><a href="http://freecad-project.de/svn/ExampleData/Examples/CAD/Complex.FCStd">Complex Part</a></li>
        </ul>
      </div>

    </div>

    <div class="column" id="description">
      &nbsp;
    </div>

    <form class="options">
      <input type="checkbox" name="closeThisDialog">
      Close this window after opening or creating a file<br/>
      <input type="checkbox" name="dontShowAgain">
      Don't show me this window again next time
    </form>

  </body>
</html>
"""

def getWorkbenches():
        return '''
        <ul>    
          <li><img src="basepath/PartDesign.png">&nbsp;
              <a onMouseover="show('<h3>Designing parts</h3> \
                              <p>The <b>Part Design</b> workbench is designed \
                              to create complex pieces based on constrained 2D sketches. \
                              Use it to draw 2D shapes, constrain some of their elements \
                              and extrude them to form 3D pieces.</p><p><small>Example \
                              workflow:</small></p><img src=basepath/PartDesignExample.png>')" 
                 onMouseout="show('')" 
                 href="basepath/PartDesign.py">Part Design</a></li>
          <li><img src="basepath/ArchDesign.png">&nbsp;
              <a onMouseover="show('<h3>Designing architectural elements</h3> \
                              <p>The <b>Architectural Design</b> workbench \
                              is specially designed for working with architectural \
                              elements such as walls or windows. Start by drawing \
                              2D shapes, and use them as guides to build architecutral \
                              objects.</p>')" 
                 onMouseout="show('')"
                 href="basepath/ArchDesign.py">Architectual Design</a></li>
          <li><img src="basepath/Mesh.png">&nbsp;
              <a onMouseover="show('<h3>Working with Meshes</h3> \
                              <p>The <b>Mesh Workbench</b> is used to work with \
                              Mesh objects. Meshes are simpler 3D objects than Part objects, \
                              but they are often easier to import and export to/from other \
                              applications.</p><p>FreeCAD offers you several tools to convert \
                              between Mesh and Part objects.</p>')" 
                 onMouseout="show('')" 
                 href="basepath/Mesh.py">Work with Meshes</a></li>
          <li><img src="basepath/Complete.png">&nbsp;
              <a onMouseover="show('<h3>A complete workbench</h3> \
                              <p>This is the <b>FreeCAD default workbench</b>, \
                              populated with some of the most commonly used tools.</p>')" 
                 onMouseout="show('')" 
                 href="basepath/DefaultWorkbench.py">The Default Workbench</a></li>
        </ul>
'''

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
                html += "<p>file size: " + getSize(s.st_size) + "<br/>"
                html += "creation time: " + getLocalTime(s.st_ctime) + "<br/>"
                html += "last modified: " + getLocalTime(s.st_mtime) + "</p>"
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
                        html += 'href="basepath/LoadMRU'+str(i)+'.py">'
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
        
        # replace paths
        basepath = "file://" + os.path.dirname( __file__)
        html = html.replace("basepath",basepath)
        
	return html
	
