////////////////////////////////////////////////////////////////////////
//
// This file SavePic.java is part of SURFEX.
//
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// 
// SURFEX version 0.90.00
// =================
//
// Saarland University at Saarbruecken, Germany
// Department of Mathematics and Computer Science
// 
// SURFEX on the web: www.surfex.AlgebraicSurface.net
// 
// Authors: Oliver Labs (2001-2008), Stephan Holzer (2004-2005)
//
// Copyright (C) 2001-2008
// 
// 
// *NOTICE*
// ========
//  
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation ( version 3 or later of the License ).
// 
// See LICENCE.TXT for details.
// 
/////////////////////////////////////////////////////////////////////////

import java.awt.Dimension;
import java.io.BufferedReader;
import java.io.FileOutputStream;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.URL;
import java.net.URLConnection;
import java.net.URLEncoder;
import java.util.*;

//////////////////////////////////////////////////////////////
//
// class SavePic
//
//////////////////////////////////////////////////////////////

public class SavePic extends Thread {
  String filelocation;

  boolean dithered;

  boolean antialiasing;

  int width;

  int height;

  int dpi;

  
  surfex surfex_;
  
  jv4surfex jv4sx;

  Project project;

  double[] CamPos;

  double[] viewDir;
  
  jv4surfex jv4sx_project;

  double[] upVector;

  double[] rightVector;

  String rotateString = "rot_x=0.0; rot_y=0.0; rot_z=0.0;\n";// default

  String[] runningParams;

  double[] runningParamsValue;
  
  Vector Lamps;
  
  boolean _3d=false;
  boolean stereo_redGreen=true;

  SavePic(String filelocation, boolean dithered, boolean antialiasing,
      int height, int width, int dpi, surfex su, Project pro,
      double[] CamPos, double[] viewDir, double[] upVector,
      double[] rightVector, String[] runningParams,
      double[] runningParamsValue, jv4surfex jv4sx, Vector Lamps) {
    this.filelocation = filelocation;
    this.dithered = dithered;
    this.antialiasing = antialiasing;
    this.width = width;
    this.height = height;
    this.dpi = dpi;
    this.surfex_ = su;
    this.project = pro;
    this.CamPos = CamPos;
    this.viewDir = viewDir;
    this.upVector = upVector;
    this.rightVector = rightVector;
    this.runningParams = runningParams;
    this.runningParamsValue = runningParamsValue;
    this.jv4sx=jv4sx;
    this.Lamps=Lamps; 
    //  System.out.println("width:"+width+",
    // height:"+height+"dithered:"+this.dithered);
    //  System.out.println("fl:"+this.filelocation);
  }
  
  SavePic(String filelocation, boolean dithered, boolean antialiasing,
        int height, int width, int dpi, surfex su, Project pro,
        double[] CamPos, double[] viewDir, double[] upVector,
        double[] rightVector, String[] runningParams,
        double[] runningParamsValue, jv4surfex jv4sx, Vector Lamps, jv4surfex jv4sx_project) {
      this.filelocation = filelocation;
      this.dithered = dithered;
      this.antialiasing = antialiasing;
      this.width = width;
      this.height = height;
      this.dpi = dpi;
      this.surfex_ = su;
      this.project = pro;
      this.CamPos = CamPos;
      this.viewDir = viewDir;
      this.upVector = upVector;
      this.rightVector = rightVector;
      this.runningParams = runningParams;
      this.runningParamsValue = runningParamsValue;
      this.jv4sx=jv4sx;
      this.jv4sx_project=jv4sx_project;
      this.Lamps=Lamps; 
      //  System.out.println("width:"+width+",
      // height:"+height+"dithered:"+this.dithered);
      //  System.out.println("fl:"+this.filelocation);
    }
  

  /*SavePic(String filelocation, boolean dithered, boolean antialiasing,
      int height, int width, int dpi, surfex su, Project pro,
      double[] CamPos, double[] viewDir, double[] upVector,
      double[] rightVector,jv4surfex jv4sx, Vector Lamps) {
    this.filelocation = filelocation;
    this.dithered = dithered;
    this.antialiasing = antialiasing;
    this.width = width;
    this.height = height;
    this.dpi = dpi;
    this.surfex_ = su;
    this.project = pro;
    this.CamPos = CamPos;
    this.viewDir = viewDir;
    this.upVector = upVector;
    this.rightVector = rightVector;
    this.runningParams = null;
    this.runningParamsValue = null;
    this.jv4sx=jv4sx;
    this.Lamps=Lamps;
    //  System.out.println("width:"+width+",
    // height:"+height+"dithered:"+this.dithered);
    //  System.out.println("fl:"+this.filelocation);
  }*/
  
  public void set3d(boolean b){
  	set3drg(b);
  }
  
  public void set3drg(boolean b){
  	_3d=b;
  }
  
  public void set3drb(boolean b){
  	_3d=b;
  }
  
  public void setStereoRedGreen(boolean b){
  	stereo_redGreen=b;
  }

  public void RunWithoutOwnThread() {
    CalcAndSave();
  }
    
    public void setRotateString(String r) {
	rotateString = r;
	//  System.out.println(r);
	
    }
    
    public String get3dCode_surf(double stereo_eyedist, double stereo_z,
				 boolean redGreen) {
	// redGreen =true -> verwendet rot-gruenbrille
	// redGreen =false -> verwendet rot-blaubbrille
	
	String str = "// produce 3d-image:\n";
	str += "stereo_eye =" + stereo_eyedist + ";\n";
	str += "stereo_z =" + stereo_z + ";\n";
	str += "stereo_red =1.0;\n";
	if (redGreen) {
	    str += "stereo_green =1.0;\n";
	    str += "stereo_blue =0.0;\n";
	} else {
	    str += "stereo_green =0.0;\n";
	    str += "stereo_blue =1.0;\n";
	}
	
	return str;
    }
    
    public String getLampCode_surf(Vector Lamps){
  	String str="";
	Lamp tempLamp;
	int i=0;
	ListIterator li=Lamps.listIterator();
	while(li.hasNext()){
	    tempLamp=(Lamp)li.next();
	    i++;
	    str+=tempLamp.getSurfCode(i, runningParams,runningParamsValue);
	}
	
	// System.out.println(str);
	return str;
	
    }
    
    public String getLampCode_pov(Vector Lamps){    
	System.out.println("ERROR: Methode muss noch geschrieben werden:getLampCode_pov(Vector Lamps)");
	return null;
    }
    
    
    public void run() {
	CalcAndSave();
    }
    
    private void CalcAndSave() {
	project.saveDone = false;
	
	//    if (eqAdm.raytracer == eqAdm.rtPOVRAY) {
	if (surfex_.configFrame.pov.isSelected()) {
	    /*
	    project.eqAdm.savePovCode(surfex_.tmpDir + "tmp_surfex.pov");
	    */
	} // end if(raytracer == rtPOVRAY)
	
	if (surfex_.configFrame.surf.isSelected()) {
	    surf();
	    
	}
	
	if (filelocation != surfex_.tmpDir + "test.png") {
	    if (surfex_.configFrame.pov.isSelected()
		&& surfex_.OS == surfex_.osWINDOWS) {
		// musste test.png nach filelocation hin kopieren ...
		try {
		    Runtime r = Runtime.getRuntime();
		    Process p;
		    p = r.exec("iview\\i_view32.exe " + surfex_.tmpDir
			       + "test.png /convert=" + filelocation);
		    p.waitFor();
		} catch (Exception er) {
		    System.out.println(er);
		}
	    }
	}
	project.saveDone = true;
    } // end of run()
    
    public String surfcodePart2_surfWeb(boolean dithered, String filelocation) {
	
	String surfCodePart2 = project.eqAdm.getSurfCode_Part2(filelocation);
	if (dithered) {
	    surfCodePart2 = project.eqAdm.getSurfCode_Dither_Part2();
	} else {
	    surfCodePart2 = project.eqAdm.getSurfCode_Part2(filelocation);
	}
	
	return surfCodePart2;
    }
    
    public String surfcodePart1_surfWeb(boolean dithered, String filelocation, int dpi) {
	String surfCodePart1;
	if (dithered) {
	    surfCodePart1 = surfcodePart1_surfWeb(false, filelocation, dpi)
		+ "tst.jpg" + surfcodePart2_surfWeb(false, filelocation)
		+ project.eqAdm.getSurfCode_Dither_Part1(dpi);
	    
	} else {
	    double[] zusRot;
	    if(jv4sx_project!=null)
	    {
    		zusRot=jv4sx_project.getCameraRotationYXZ();
	    }else{
    		zusRot=new double[3];
    		zusRot[0]=0.0;
    		zusRot[1]=0.0;
    		zusRot[2]=0.0;
	    }
	    
	    surfCodePart1 = project.eqAdm.getSurfCode_Part1(new Dimension(width, height), 
							    antialiasing, 
							    rotateString, 
							    runningParams,
							    runningParamsValue,
							    jv4sx,
							    zusRot);
	}
	return surfCodePart1;
    }
    
    public void surfWeb() {
	
	//     compute via the internet:
	Runtime r = Runtime.getRuntime();
	Process p;
	try {
	    
	    project.eqAdm.updateScale();
	    String surfCodePart1 = surfcodePart1_surfWeb(dithered,
							 filelocation, dpi);
	    String surfCodePart2 = surfcodePart2_surfWeb(dithered, filelocation);
	    
	    URL url = new URL(surfex_.webPrgs.surfPath.getText()
			      + "surfex_comp.php");
	    URLConnection connection = url.openConnection();
	    connection.setUseCaches(false);
	    connection.setDoOutput(true);
	    
	    PrintWriter out = new PrintWriter(connection.getOutputStream());
	    //        System.out.println("old
	    // subdir:"+surfex_.getCurrentProject().old_tmpsubdir);
	    out.print("prefix=surfex_tmp" + "&filelocation="
		      + URLEncoder.encode(filelocation) + "&tmpsubdir="
		      + URLEncoder.encode(project.old_tmpsubdir)
		      + "&prg=surf" + "&img_width=" + width + "&img_height="
		      + height + "&prg_code_part1="
		      + URLEncoder.encode(surfCodePart1) + "&prg_code_part2="
		      + URLEncoder.encode(surfCodePart2));
	    out.close();
	    
	    // read the file names
	    BufferedReader in = new BufferedReader(new InputStreamReader(
						       connection.getInputStream()));
	    String inputLine;
	    inputLine = in.readLine();
	    //        System.out.println(inputLine);
	    String strFilename = "";
	    if (inputLine.equals("okay")) {
		//      System.out.println("it works:");
		//      System.out.println(in.readLine());
		strFilename = in.readLine();
		//      System.out.println("file:"+strFilename);
		project.old_tmpsubdir = in.readLine();
		//      System.out.println("subdir:"+surfex_.getCurrentProject().old_tmpsubdir);
		project.eqAdm.img_filename = surfex_.webPrgs.surfPath
		    .getText()
		    + strFilename;
	    } else {
		//      System.out.println("it does not work:");
		System.out.println(in.readLine());
	    }
	    //        System.out.println("the rest:");
	    //        while ((inputLine = in.readLine()) != null) {
	    //      System.out.println(inputLine);
	    //        }
	    in.close();
	    //        System.out.println("done.");
	    // ???
	    // end of else (i.e. we really have to recompute)
	    
	} catch (Exception er) {
	    System.out.println("SavePic:" + er.toString());
	}
    }
    
    public String getSurfCode(){
  	if (surfex_.configFrame.webPro.isSelected()) {
	    
	    return getSurfCode_Web();
	} else {
	    return getSurfCode_OnSystem();
	}
  	//return null;
    }
    
    public String getSurfCode_Web(){
  	String surfCode = "";
	if (_3d) {
	    surfCode += get3dCode_surf(5.0, 10.0, stereo_redGreen);
	}
	surfCode += getLampCode_surf(Lamps);
	
	
	surfCode += surfcodePart1_surfWeb(dithered, filelocation,dpi);
	//  String surfCodePart1 = surfcodePart1_OnSystem(dithered,
	//       filelocation);
	
	surfCode += surfcodePart2_surfWeb(dithered, filelocation);
	return surfCode;
    }
    
    public String getSurfCode_OnSystem() {
	
	String surfCode = "";
	if (_3d) {
	    surfCode += get3dCode_surf(2.0, 5.0, stereo_redGreen);
	}
	surfCode += getLampCode_surf(Lamps);
	surfCode += surfcodePart1_OnSystem(dithered, filelocation);
	//  String surfCodePart1 = surfcodePart1_OnSystem(dithered,
	//       filelocation);
	
	surfCode += surfcodePart2_OnSystem(dithered, filelocation, dpi);
	
	
	return surfCode; 
    }
    
    public String getSurfCodePart1(){
	if (surfex_.configFrame.webPro.isSelected()) {
	    return surfcodePart1_OnSystem(dithered,filelocation);
	} else {
	    return surfcodePart1_surfWeb(dithered,filelocation,dpi);
	}
    }
    
    public String getSurfCodePart2(){
	if (surfex_.configFrame.webPro.isSelected()) {
	    return surfcodePart2_OnSystem(dithered,filelocation,dpi);
	} else {
	    return surfcodePart2_surfWeb(dithered,filelocation);
	}
    }
    
    public String surfcodePart1_OnSystem(boolean dithered, String filelocation) {
	String surfCodePart1;
	
//       surfCodePart1 = surfcodePart1_OnSystem(false, filelocation)
// //          + surfex_.tmpDir + project.tmp_surfex_file
//           + surfcodePart2_OnSystem(false, filelocation, dpi)
//           + project.eqAdm.getSurfCode_Dither_Part1(dpi);
    	double[] zusRot;
    	//System.out.println("hallo");
    	if(jv4sx_project!=null)
    	{
	    zusRot=jv4sx_project.getCameraRotationYXZ();
	    //System.out.println("zusrot"+zusRot[1]);
	    
    	}else{
	    zusRot=new double[3];
	    zusRot[0]=0.0;
	    zusRot[1]=0.0;
	    zusRot[2]=0.0;
	    
    	}
    	//rotateString="rot_x="+zusRot[2]+"; rot_y="+zusRot[1]+"; rot_z="+zusRot[2]+";\n";// default
	
	if (dithered) {
	    surfCodePart1 = project.eqAdm.getSurfCode_Part1(new Dimension(
								width, height), antialiasing, rotateString, runningParams,
							    runningParamsValue,jv4sx,zusRot)
		+ "tst.jpg\";\n"
		+ project.eqAdm.getSurfCode_Dither_Part1(dpi);
	} else {
	    surfCodePart1 = project.eqAdm.getSurfCode_Part1(new Dimension(
								width, height), antialiasing, rotateString, runningParams,
							    runningParamsValue,jv4sx,zusRot)
		+ filelocation+"\";\n";
	}
	return surfCodePart1;
    }
    
    public String surfcodePart2_OnSystem(boolean dihered, String filelocation,
					 int dpi) {
	
	String surfCodePart2;
	
	if (dithered) {
	    //      //System.out.println("dithered.");
	    surfCodePart2 = filelocation + 
		project.eqAdm.getSurfCode_Dither_Part2();
	} else {
	    surfCodePart2 = project.eqAdm.getSurfCode_Part2(filelocation);
	}
	return surfCodePart2;
    }
    
  public void surfOnSystem() {

    //        //System.out.println("use local surf");
    // i.e. ! webPro.isSelected()
    Runtime r = Runtime.getRuntime();
    Process p;
  //  //System.out.println(getLampCode_surf(Lamps));
    try {
      project.eqAdm.updateScale();
      String surfCode=getSurfCode();
      // compare to old surfcode
     /* if (surfCodePart1.equals(project.old_surfCodePart1)
          && surfCodePart2.equals(project.old_surfCodePart2)
          && (!dithered)) {
        //      //System.out.println("we do not have to compute anything");
        // we do not have to recompute the same image!
        sleep(50);
      } else {*/
        //      //System.out.println("new data, so recompute the image");
        try {
	    //	    System.out.println("file1a:"+filelocation+".pic");
	    //	    System.out.println("file1b:"+toWindows(filelocation+".pic"));
	    FileOutputStream fo;
	    if(surfex_.cygwin==1) {
		 fo = new FileOutputStream(surfex.toWindows(filelocation + ".pic"));
	    } else {
		 fo = new FileOutputStream(filelocation + ".pic");
	    }
	    //	    System.out.println("file2:"+filelocation+".pic");
	    PrintWriter pw = new PrintWriter(fo, true);
	    
	    pw.println(surfCode);
	    pw.close();
        } catch (Exception e) {
	    //System.out.println(e.toString());
        }

        //        project.eqAdm.saveSurfCode("tmp_surfex.pic",
        // "./tmp_surfex.jpg");
        p = null;

        if (surfex_.OS == surfex_.osLINUX) {

          /*
           * try { Runtime r2 = Runtime.getRuntime(); Process p2; //
           * surf muss im Pfad sein ... // auf Zelenumbrueche in
           * surf-dateien achten! p2 = r2.exec("surf -n
           * \"/home/stephan/surfex/surf_tst.pic\"");
           * InputStreamReader isr = new
           * InputStreamReader(p2.getInputStream()); BufferedReader in =
           * new BufferedReader(isr); String inputLine; String str;
           * str = ""; while ((inputLine = in.readLine()) != null) {
           * str += inputLine; } // end while(in.readLine())
           * in.close(); // //System.out.println("str:" + str);
           * 
           * p2.waitFor(); } catch (Exception e1) {
           * //System.out.println(e1.toString()); //System.exit(0); }
           *  
           */

	    //	    filelocation="/tmp/test";

// 	    System.out.println("sh -exec \""+surfex_.configFrame.surfPath.getText() + " "
//               + surfex.toLinux(filelocation + ".pic")+"\"");

	    if(surfex_.cygwin==1) {
		p = r.exec("sh -exec \""+surfex_.configFrame.surfPath.getText() + " "
			   + "\\\""+surfex.toLinux(filelocation + ".pic")+"\\\"\"");
	    } else {
// 		System.out.println(surfex_.configFrame.surfPath.getText() + " "
// 			   + "\""+filelocation + ".pic\"");
		p = r.exec(surfex_.configFrame.surfPath.getText() + " "
			   + ""+filelocation + ".pic");
	    }
//           p.waitFor();
// 	  System.out.println("done.");

//           p = r.exec("sh -exec \""+surfex_.configFrame.surfPath.getText() + " "
//               + "test.pic\"");

          ////System.out.println("aufruf: "+
          // surfex_.configFrame.surfPath.getText()
          //+ " " + filelocation + ".pic" );
          //    System.in.read();
          InputStreamReader isr = new InputStreamReader(p.getInputStream());
          BufferedReader in = new BufferedReader(isr);
          String inputLine;
          String str;
          str = "";
          while ((inputLine = in.readLine()) != null) {
	      str += inputLine;
            ////System.out.println(inputLine);
          } // end while(in.readLine())
          in.close();
          p.waitFor();
	  //	  System.exit(0);
        } else {
          // ??? surf under Windows???
          //System.out.println("Os nicht cygwin ");
        }

        //        //System.out.println("running...");
        //    //System.out.println("running...");

        /*
         * InputStreamReader isr = new InputStreamReader(p
         * .getErrorStream()); //System.out.println("2,705" );
         * BufferedReader in = new BufferedReader(isr); String
         * inputLine; String str; str = ""; //System.out.println("2,71" );
         * while ((inputLine = in.readLine()) != null) {
         * 
         * str += inputLine; //System.out.println("str:"+str+"."); //
         * //System.out.println("done."); //
         * //System.out.println("convert..."); // p = r.exec("convert
         * test.png test.gif"); // p.waitFor(); } // end
         * while(in.readLine()) in.close(); //
         * //System.out.println("closed."); p.waitFor(); //
         * //System.out.println("ended.");
         * 
         * //System.out.println("2,8" );
         */
        if (dithered) {
          project.eqAdm.img_filename = surfex_.tmpDir
              + project.tmp_surfex_file;
        } else {
          project.eqAdm.img_filename = filelocation;
        }
      //}
    } catch (Exception er) {
      //System.out.println("SavePic:" + er.toString());
    }

  } // end else

  
    public void updateLamps(Vector la){
  	Lamps=la;
    }
    
    
    public void surf() {
//	System.out.println("up:"+upVector[0]+","+upVector[1]+","+upVector[2]);
	if (surfex_.configFrame.webPro.isSelected()) {
	    surfWeb();
	} else {
	    surfOnSystem();
	}
	
    }
    
    public void Povray() {
	Runtime r = Runtime.getRuntime();
	Process p;
	//     rufe povray auf und speichere Ausgabe als Bild;
	try {
	    
	    //         p = r.exec("C:\\Programme\\POV-Ray for Windows
	    // v3.6\\bin\\pvengine /NR +Otest.gif -D /RENDER tmp_surfex.pov
	    // /EXIT");
	    //      //System.out.println("povray...");
	    //   String cmds[] = { "povray +Otest.png +Itmp_surfex.pov
	    // /etc/povray.ini"; };
	    
	    if (surfex_.OS == surfex_.osLINUX) {
		p = r.exec("povray +H240 +W240 +O" + surfex_.tmpDir
			   + "test.png +I" + surfex_.tmpDir
			   + "tmp_surfex.pov povray.ini");
	    } else {
		//if(surfex_.OS==surfex_.osWINDOWS){
		//p = r.exec("C:\\Programme\\POV-Ray for Windows
		// v3.6\\bin\\pvengine /NR +H480 +W480 +Otest.png -D /RENDER
		// tmp_surfex.pov povray.ini /EXIT");
		p = r.exec(surfex_.configFrame.povPath.getText() + " /NR +H"
			   + height + " +W" + width + " +O" + filelocation
			   + " -D /RENDER " + surfex_.tmpDir
			   + "tmp_surfex.pov /EXIT");
		p.waitFor();
	    }
	    //      //System.out.println("running...");
	    InputStreamReader isr = new InputStreamReader(p.getErrorStream());
	    BufferedReader in = new BufferedReader(isr);
	    String inputLine;
	    String str;
	    str = "";
	    while ((inputLine = in.readLine()) != null) {
		str += inputLine;
		//      //System.out.println("str:"+str+".");
		in.close();
		
		//      //System.out.println("closed.");
		p.waitFor();
		//      //System.out.println("convert...");
		//      p = r.exec("convert test.png test.gif");
		//      p.waitFor();
	    }
	} catch (Exception e1) {
	    //System.out.println("povray problem.");
	    //System.out.println(e1.toString());
	}
    }
} // end class SavePic

