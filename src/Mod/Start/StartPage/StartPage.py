import FreeCAD,FreeCADGui

page = """
<html>
  <head>
    <title>FreeCAD - Start page</title>
    <style type="text/css">
      body {
        background: #171A2B url(Background.jpg);
        color: white;
        font-family: Arial, Helvetica, Sans;
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
      h1 {
        font-size: 3em;
        letter-spacing: 2px;
        padding: 20px 0 0 80px;
      }
      h2 {
        padding-left: 40px;
        font-size: 1.2em;
      }
      ul {
        list-style-type: none;
      }
      .column {
        width: 340px;
        float: left;
        margin-left: 10px;
      }
      .block {
        background: rgba(30,31,33,0.6);;
        border-radius: 5px;
        padding: 8px;
        margin-bottom: 10px;
      }
    </style>
  </head>

  <body>

    <h1>FreeCAD Start Center</h1>

    <div class="column">

      <div class="block">
        <h2>What do you want to do?</h2>
        <ul>    
          <li><a href="PartDesign.py">Part Design</a></li>
          <li><a href="ArchDesign.py">Architectual Design</a></li>
          <li><a href="Mesh.py">Work with meshes</a></li>
        </ul>
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
          <li><a href="http://free-cad.sf.net/">FreeCAD Homepage</a></li>
        </ul>
      </div>

      <div class="block">
        <h2>Example projects</h2>
        <ul>
          <li><a href="LoadSchenkel.py">Schenkel STEP file</a></li>
          <li><a href="LoadPartDesignExample.py">Load a PartDesign example</a></li>
          <li><a href="LoadDrawingExample.py">Load a Drawing extraction</a></li>
          <li><a href="LoadRobotExample.py">Load a Robot simulation example</a></li>
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

  </body>
</html>

"""

def handle():
	return page
	
