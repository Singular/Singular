////////////////////////////////////////////////////////////////////////
//
// This file SaveMovie.java is part of SURFEX.
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

import java.util.*;
import java.text.NumberFormat;
import java.awt.*;

import javax.swing.JComponent;
import javax.swing.*;

public class SaveMovie extends Thread {

  int N;

  // Rotation types:
  //final int ROTATE_X_AXIS = 1;

//  final int ROTATE_Y_AXIS = 2;

  //final int ROTATE_Z_AXIS = 3;

  //final int ROTATE_XY_AXIS = 4;

//  final int ROTATE_XZ_AXIS = 5;

  //final int ROTATE_YX_AXIS = 6;

  //final int ROTATE_YZ_AXIS = 7;

  //final int ROTATE_ZX_AXIS = 8;

  //final int ROTATE_ZY_AXIS = 9;

  String SaveToFileLocation;

  boolean dithered, antialiasing;

  surfex surfex_;

  int fps;//, dpi;

  int T;

  int omitType;

  boolean _3d;

  int RotType;

  int height, width;

  SavePic savePic;

  Project pro;

  double[] CamPos;

  double[] viewDir;

  double[] upVector;

  double[] rightVector;

  int Xrot, Yrot, Zrot;

  String[] parametersToRun;
  
  int rand;

  public SaveMovie(String SaveToFileLocation, surfex su, int FramesPerSecond,
		   int dpi, boolean dithered, boolean antialiasing, int Time,
		   int omitType,
		   int height, int width, boolean _3d, int RotType, Project pro,
		   double[] CamPos, double[] viewDir, double[] upVector,
		   double[] rightVector, int Xrot, int Yrot, int Zrot,
		   String[] parametersToRun) {
      this.SaveToFileLocation = SaveToFileLocation;
      this.surfex_ = su;
      this.fps = FramesPerSecond;
      this.T = Time;
      this.omitType = omitType;
      this._3d = _3d;
      this.RotType = 1;//RotType;
      this.height = height;
      this.width = width;
      this.pro = pro;
      this.dithered = dithered;
      this.antialiasing = antialiasing;
      this.width = width;
      this.height = height;
      //  this.dpi = dpi;
      this.CamPos = CamPos;
      this.viewDir = viewDir;
      this.upVector = upVector;
      this.rightVector = rightVector;
      this.Xrot = Xrot;
      this.Yrot = Yrot;
      this.Zrot = Zrot;
      this.parametersToRun = parametersToRun;
      
//     System.out.println(SaveToFileLocation + "SaveToFileLocation");
//     System.out.println(this.surfex_ + "su");
//     System.out.println(this.fps + "FramesPerSecond");
//     System.out.println(this.T + "Time");
//     System.out.println(this._3d + "_3d");
//     System.out.println(this.RotType + "RotType");
//     System.out.println(this.height + "height");
//     System.out.println(this.width + "width");
//     System.out.println(this.pro + "pro");
//     System.out.println(this.dithered + " dithered");
//     System.out.println(this.antialiasing + " antialiasing");
//     System.out.println(this.width + " width");
//     System.out.println(this.height + " height");
//     System.out.println(this.dpi + " dpi");
//     System.out.println(this.CamPos[0] + "CamPos 0");
//     System.out.println(this.viewDir[0] + "viewDir0");
//     System.out.println(this.upVector[0] + "upVector0");
//     System.out.println(this.rightVector[0] + "rightVector0");
//     System.out.println(this.Zrot + " xrot");
//     System.out.println(this.Yrot + " yrot");
//     System.out.println(this.Zrot + " zrot");

      N = fps * T;
      
      rand=((int)(Math.random()*2000000000));
      //RotTypeROTATE_y_AXIS();//pro.jv4sx.getCamPos(),pro.jv4sx.getViewDir(),pro.jv4sx.getUpVector(),pro.jv4sx.getRightVector());
      
  }
    
    public void run() {
//     System.out.println("saveMovie gestartet");
	MakeMovie(StandardRot(Xrot, Yrot, Zrot));//RotTypeROTATE_y_AXIS());
    }
    
    private LinkedList getRotStrings(int rotType) {
	switch (rotType) {
	    //case ROTATE_X_AXIS:
	    // return StandardRot(Xrot, Yrot, Zrot);
	    //    case ROTATE_Y_AXIS: return RotTypeROTATE_X_AXIS();
	    //  case ROTATE_Z_AXIS: return RotTypeROTATE_Z_AXIS();
	}
	return null; // wird aber nicht vorkommen
	
    }
    
    private LinkedList StandardRot(int Xrot, int Yrot, int Zrot) {
	LinkedList rotStrings = new LinkedList();
	double tmp;
	for (int i = 0; i < N; i++) {
	    tmp = ((double) i) / N * 2 * Math.PI;
	    rotStrings.add("rot_x=" + (tmp * Xrot) + "; rot_y=" + (tmp * Yrot)
			   + "; rot_z=" + (tmp * Zrot) + ";\n");
	}
	return rotStrings;
    }
    
    private void MakeMovie(LinkedList rotateStrings){//double[] camPos, double[]
	// viewDir,double[]
	// upVector,double[]
	// rightVector){
	// eine Drehung produzieren:
	
	int i=-1;
	
	JFrame pframe=createAndShowGUI(N);
	
	double[] runningParametersValue=new double[N];
	
	NumberFormat nf=NumberFormat.getNumberInstance(); 
	nf.setMinimumIntegerDigits(3); // ggf. mit fuehrenden Nullen ausgeben
	ListIterator rotateStringsIterator=rotateStrings.listIterator();
	OneParameter tmpParam;
	
	while(rotateStringsIterator.hasNext() && 
	      (!((ProgressFrame)pframe.getContentPane()).processCanceled)){ 
	    i++; 
	    
	    int j;
	    //System.out.println("calc parameters:"+parametersToRun.length);
	    for(j=0;j<parametersToRun.length;j++){
		tmpParam=pro.parAdmin.getParameter(parametersToRun[j]);
//		System.out.println("from:"+tmpParam.newFrom+", to:"+tmpParam.newTo);
//		System.out.println("i:"+i+", N-1:"+(N-1));
		runningParametersValue[j]=(tmpParam.newFrom+
					   (tmpParam.newTo-tmpParam.newFrom)*i/(N-1));
//		System.out.println("param: "+j+" :"+parametersToRun[j]+" value:"+
//				   runningParametersValue[j]);
	    }
	    
	    //System.out.println((i+1)+" / "+N );
	    if(omitType==0 || 
	       (omitType==1 && i+1>1) || 
	       (omitType==2 && i+1<N)) {
		if(surfex_.cygwin==1) {
		    ((ProgressFrame)pframe.getContentPane()).refresh(i+1,
								     "Creating Frame "+(i+1)+"/"+N,
								     surfex_.toWindows(surfex_.tmpDir+"tmp_surfex_MoviePic_"+rand+"_"+nf.format(i-1)+".jpg"));
		} else {
		    ((ProgressFrame)pframe.getContentPane()).refresh(i+1,"Creating Frame "+(i+1)+"/"+N,
								     surfex_.tmpDir+"tmp_surfex_MoviePic_"+rand+"_"+nf.format(i-1)+".jpg");
		}
		try {
		    SavePic save=new SavePic(surfex_.tmpDir+"tmp_surfex_MoviePic_"+rand+"_"+nf.format(i)+".jpg",false,
					     antialiasing,height, width,300, surfex_, pro, CamPos, 
					     viewDir, upVector, rightVector, parametersToRun, runningParametersValue, 
					     surfex_.jv4sx, pro.getAllLamps() );
		    save.setRotateString((String) rotateStringsIterator.next());
		    save.set3d(this._3d);
		    //  System.out.println(save.rotateString);
		    save.start();
		    save.join();
		    sleep(20); // damit das System auch noch was anderes machen kann
		} catch(Exception e){
		    System.out.println("exception in saveThread - saveMovie"+e);
		}
	    } else {
		rotateStringsIterator.next();
//		System.out.println("omit:"+omitType+", i:"+i);
	    }
	} // end while()

	
	if(((ProgressFrame)pframe.getContentPane()).processCanceled){
	    // user hat Prozess abgebrochen
	}else{
	    ((ProgressFrame)pframe.getContentPane()).refresh(i+1+(N/20),"producing film from frames...");
	    
	    // convert the .jpg-files into a movie file:
//     System.out.println("try convert...");
	    try {
//         System.out.println("convert...");
		Runtime r = Runtime.getRuntime();
		int delay = 60 / fps;
		//   
// 	System.out.println("sh -exec \"convert -delay "+delay+" -loop 0 "+surfex_.tmpDir+"tmp_surfex_MoviePic_"+rand+"*.jpg \\\""+surfex.toLinux(SaveToFileLocation)+"\\\"\"");
// 	System.out.println("sh -exec \"convert -delay "+delay+" -loop 0 "+
// 			   surfex.toWindows(surfex_.tmpDir+"tmp_surfex_MoviePic_"+rand+"*.jpg") +" \\\""+
// 			   surfex.toWindows(SaveToFileLocation)+"\\\"\"");
		Process p;
		if(surfex_.cygwin==1) {
		    //	    p = r.exec("sh -exec \"convert -delay "+delay+" -loop 0 "+surfex_.tmpDir+"tmp_surfex_MoviePic_"+rand+"*.jpg \\\""+surfex.toLinux(SaveToFileLocation)+"\\\"\"");
		    p = r.exec("sh -exec \"convert -delay "+delay+" -loop 0 \\\""+
			       (surfex.toWindows(surfex_.tmpDir+"tmp_surfex_MoviePic_"+rand+"_*.jpg")).replaceAll("\\\\","\\\\\\\\") +"\\\" \\\""+
			       (surfex.toWindows(SaveToFileLocation)).replaceAll("\\\\","\\\\\\\\")+"\\\"\"");
		} else {
//		    System.out.println("try to save movie to:"+
//				       SaveToFileLocation.replaceAll(" ", "\\\\ ")+".");
		    p = r.exec("convert -delay "+delay+" -loop 0 "+surfex_.tmpDir+
			       "tmp_surfex_MoviePic_"+rand+"_*.jpg "+
			       SaveToFileLocation.replaceAll(" ", "\\ ")+"");
		}
		p.waitFor();
//         String tstr="sh -exec \"convert prevstart.jpg hhhhhhhhhhhhh.gif\"";
//         Process p = r.exec(tstr);
//         System.out.println(tstr);//+SaveToFileLocation);
	    } catch (Exception e1) {
		System.out.println("convert problem.");
		System.out.println(e1.toString());
	    }   
//     System.out.println("convert done.");
	    
	    
	    ((ProgressFrame)pframe.getContentPane()).refresh((int)(N*1.2),"Film was produced successfully");
	    
	    
	}
	pframe.dispose();
	
    } 
    
    private JFrame createAndShowGUI(int N) {
    //Make sure we have nice window decorations.
    // JFrame.setDefaultLookAndFeelDecorated(true);

    //Create and set up the window.
    JFrame frame = new JFrame("creating " + SaveToFileLocation
        + " movie ...");
    // frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

    //Create and set up the content pane.
    ProgressFrame pframe = new ProgressFrame((int) (N * 1.2));
    JComponent newContentPane = pframe;

    //newContentPane.setOpaque(true); //content panes must be opaque
    frame.setContentPane(newContentPane);

    //Display the window.
    if(surfex_.showMoviePreview) {
	frame.pack();
	frame.setVisible(true);
    }
    return frame;
    //JFrame s=new AskForRatioDialog(0.0);
    //  Thread s=new ProgressBarThread();
  }
}
