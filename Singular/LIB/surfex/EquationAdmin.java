////////////////////////////////////////////////////////////////////////
//
// This file EquationAdmin.java is part of SURFEX.
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

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedReader;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.ListIterator;
import java.util.Vector;
import jv.vecmath.*;

import javax.swing.JPanel;
import javax.swing.SwingUtilities;

//////////////////////////////////////////////////////////////
//
// class EquationAdmin
//
//////////////  ////////////////////////////////////////////////
 
public class EquationAdmin extends JPanel implements ActionListener {
	//   Anfang Variablen
	// GUI
	Equation oldeq;
 
	public String img_filename;

	JPanel eqnpanel = new JPanel();

	RayFrame rayFrame;

	// Raytracer
	int rtPOVRAY = 0;

	int rtSURF = 1;

	int raytracer = rtSURF;

	public jv4surfex jv4sx;

	private surfex surfex_;

	private Project project;

	private double[] ang = new double[3];

	private double[] scale = new double[3];

	// this Project
	Vector eqnList = new Vector();

	int eqncounter = 0;

	int ProjectNumber;

	String projectName;

	String filename;

	PdVector lastUpVector;//=cam.getUpVector();

	PdVector lastViewDir;//=cam.getViewDir();

	PdVector lastPosition;//=cam.getPosition();

	PdVector lastInterest;

	double lastScale;//=cam;//.getUpScale();

	//Ende Variablen

	// Konstruktoren
	EquationAdmin(jv4surfex jv4sx, RayFrame ray, surfex su, Project myp) {
		project = myp;
		surfex_ = su;
		img_filename = surfex_.tmpDir + "test.png";
		this.jv4sx = jv4sx;
		rayFrame = ray;
		projectName = "Project 1";

		lastScale = 1.0;
		lastViewDir = new PdVector(3.2, 0.3, -0.9);
		lastPosition = new PdVector(10.0, 20.0, -20.0);
		lastUpVector = new PdVector(0.0, 1.0, 0.0);
		lastInterest = new PdVector(0.0, 1.0, 0.0);

		ang[0] = 3.2;
		ang[1] = 0.3;
		ang[2] = -0.9;
		scale[0] = lastScale;
		scale[1] = lastScale;
		scale[2] = lastScale;

		//updateJV4SX(); 
 
	}

    public void clear() {
	eqnList.clear();
	eqncounter = 0;
	eqnpanel.removeAll();
    }
 
    public Vector getEqNrs() {
	Vector temp = new Vector();
	temp.add("-"); 
	ListIterator li = eqnList.listIterator();
	while (li.hasNext()) {
	    temp.add(new Integer(((Equation) li.next()).eqnr));
	}
	return temp;
    }
    
    public Equation getEqnFromNo(int no) {
	ListIterator li = eqnList.listIterator();
	Equation tmpEq;
	while (li.hasNext()) {
	    tmpEq = (Equation) li.next();
	    if (tmpEq.eqnr == no) {
		return (tmpEq);
	    }
	}
	return (null);
    }
    
    EquationAdmin(int i, String filename, RayFrame ray, surfex su,
		  jv4surfex jv4sx, Project myp) {
	project = myp;
	this.jv4sx = jv4sx;
	//	System.out.println("a"+vec2Str(jv4sx.getCamPos()));
	
	this.filename = filename;
	rayFrame = ray;
	ProjectNumber = i;
	projectName = "Project" + i;
	this.surfex_ = su;
	img_filename = surfex_.tmpDir + "test.png";
	
	//savePicDialog = new SavePicDialog(projectName, rayFrame, this, surfex_);
	// es muss vorher der Projectname da sein
	//updateJV4SX();
	lastScale = 1.0;
	lastViewDir = new PdVector(3.2, 0.3, -0.9);
	lastPosition = new PdVector(10.0, 20.0, -20.0);
	lastUpVector = new PdVector(0.0, 1.0, 0.0);
	lastInterest = new PdVector(0.0, 1.0, 0.0);
	
	ang[0] = 3.2;
	ang[1] = 0.3;
	ang[2] = -0.9;
	scale[0] = lastScale;
	scale[1] = lastScale;
	scale[2] = lastScale;
	
	this.setLayout(new BorderLayout());
	
	eqnpanel.setLayout(new GridLayout(20, 0));
	newEquation();
	
	oldeq = (Equation) eqnList.firstElement();
	//  ((Equation) eqnList.firstElement()).setBasePlane(true, jv4sx, scale);
	((Equation) eqnList.firstElement()).setBasePlaneDefault();
	this.add(eqnpanel);
    }
    
    // Ereignismethoden
    public void actionPerformed(ActionEvent e) {
	String command = e.getActionCommand();
	//  System.out.println(command );
	
	if (command.length() >= 15
	    && command.substring(0, 15).hashCode() == "basePlaneButton"
	    .hashCode()) {
	    // normalen Vergleich will er aus irgendeinem Grund nicht, desshalb muss es halt ueber den hashcode gehen ...
	    // klar: basePlaneButton
	    //      System.out.println("plan");
	    ListIterator li = eqnList.listIterator();
	    Equation eq;
	    oldeq.setBasePlane(false);
	    while (li.hasNext()) {
		eq = ((Equation) li.next());
		if (eq.basePlaneButton.getActionCommand() == command) {
		    // fertig
		    oldeq = eq;
		    updateScale();
		    eq.setBasePlane(true, jv4sx, scale);
		}
	    }
	}
	//  System.out.println(command);
	if (command.length() >= 17
	    && command.substring(0, 17).hashCode() == "deletePlaneButton"
	    .hashCode()) {
	    // normalen Vergleich will er aus irgendeinem Grund nicht, desshalb muss es halt ueber den hashcode gehen ...
	    // klar: deletePlaneButton
	    int internalEqnr = (new Integer(command.substring(17, command
							      .length()))).intValue();
	    if (((Equation) eqnList.elementAt(internalEqnr - 1)).isBasePlane()) {
		// dann muss ne andere die BasePlane werden
		updateScale();
		if (internalEqnr != 1) {
		    ((Equation) eqnList.elementAt(internalEqnr - 2))
			.setBasePlane(true, jv4sx, scale);
		} else {
		    if (eqnList.size() >= 2) {
			// wurde die erste geloescht, setze die naechste als Baseplane
			((Equation) eqnList.elementAt(internalEqnr))
			    .setBasePlane(true, jv4sx, scale);
		    }
		    // else sind keine Planes mehr da!
		}
	    }
	    eqnList.removeElementAt(internalEqnr - 1);
	    //      System.out.println("soze" + eqnList.size());
	    updateEquationPanel();
	}
    }
    
    /*
    public void savePovCode(String filename) {
	// erzeuge POVCode und
	try {
	    FileOutputStream fo = new FileOutputStream(filename);
	    
	    PrintWriter pw = new PrintWriter(fo, true);
	    //pw.println("String");
	    //      pw.println("camera { location 20*z look_at <0,0,0> angle 45 up <0,1,0> right <1,0,0>}");
	    pw.println("camera { location " + vec2Str(jv4sx.getCamPos())
		       + " direction " + vec2Str(jv4sx.getViewDir())
		       + " angle 45 up " + vec2Str(jv4sx.getUpVector())
		       + " right " + vec2Str(jv4sx.getRightVector()) + "}");
	    pw.println("light_source { <100,100,-100> .5 }"); // the main light
	    pw.println("light_source { <100,-100,100> .5 }"); // the main light
	    pw.println("light_source { <-100,100,100> .5 }"); // the main light
	    pw.println("light_source { <100,0,0> .25 shadowless}");
	    pw.println("light_source { <-100,0,0> .25 shadowless}");
	    pw.println("light_source { <0,100,0> .25 shadowless}");
	    pw.println("light_source { <0,-100,0> .25 shadowless}");
	    pw.println("light_source { <0,0,100> .25 shadowless}");
	    pw.println("light_source { <0,0,-100> .25 shadowless}");
	    pw.println("light_source { <100,100,0> .25 shadowless}");
	    pw.println("light_source { <-100,100,0> .25 shadowless}");
	    pw.println("light_source { <100,-100,0> .25 shadowless}");
	    pw.println("light_source { <-100,-100,0> .25 shadowless}");
	    pw.println("light_source { <0,100,100> .25 shadowless}");
	    pw.println("light_source { <0,-100,100> .25 shadowless}");
	    pw.println("light_source { <0,100,-100> .25 shadowless}");
	    pw.println("light_source { <0,-100,-100> .25 shadowless}");
	    pw.println("light_source { <100,0,100> .25 shadowless}");
	    pw.println("light_source { <-100,0,100> .25 shadowless}");
	    pw.println("light_source { <100,0,-100> .25 shadowless}");
	    pw.println("light_source { <-100,0,-100> .25 shadowless}");
	    pw.println("background { rgb <.0,.0,.0> }");
	    pw.println("#declare Pi=3.141592654;");
	    // vom jv 3 zahlen:
	    //      ang = jv4sx.getCameraRotationYXZ();
	    //      pw.println("#declare degy = "+(180.0*ang[0]/Math.PI)+";");
	    //      pw.println("#declare degx = "+(180.0*ang[1]/Math.PI)+";");
	    //      pw.println("#declare degz = "+(-180.0*ang[2]/Math.PI)+";");
	    pw.println("#declare degy = " + (180.0 * 0.0 / Math.PI) + ";");
	    pw.println("#declare degx = " + (180.0 * 0.0 / Math.PI) + ";");
	    pw.println("#declare degz = " + (-180.0 * 0.0 / Math.PI) + ";");
	    //      vom slider zoom.. >0 zwischen (min,max) , min,max eingebbar    , erstmal 0-5  langer slider
	    pw.println("#declare scale_x = 1.0;");
	    pw.println("#declare scale_y = 1.0;");
	    pw.println("#declare scale_z = 1.0;");
	    pw.println("#declare Radius=4.5;");
	    
	    // Polynome aus der Liste uebergebn
	    ListIterator li = eqnList.listIterator();
	    Equation thisEqn;
	    while (li.hasNext()) {
		thisEqn = (Equation) li.next();
		if (thisEqn.isSelected()) {
		    thisEqn.savePOVCode(pw);
		}
	    }
	    pw.close();
	} catch (IOException er) {
	    System.out.println(er);
	}
	
    }
    */
    
    public void updateScale() {
	scale[0] = 1.0 / jv4sx.getScale();
	scale[1] = 1.0 / jv4sx.getScale();
	scale[2] = 1.0 / jv4sx.getScale();
    }
    
    public String fileFormatFromExt(String strFilename) {
//	System.out.println("str:"+strFilename);
	int ind = strFilename.lastIndexOf(".");
	if (ind == -1) {
	    return ("");
	} else {
	    String strFormat = strFilename.substring(ind + 1);
//	    System.out.println("strFormat:"+strFormat+".");
	    if (strFormat.equals("ras")) {
		return ("sun");
	    } else if (strFormat.equals("tif")) {
		return ("tiff");
	    } else if (strFormat.equals("JPG")) {
//		System.out.println("strFormat1:"+strFormat+".");
		return ("jpg");
	    } else {
//		System.out.println("strFormat2:"+strFormat+".");
		return (strFormat);
	    }
	}
    }
    
    public String getSurfCode_Part1(Dimension d, boolean antialiasing,
				    jv4surfex jv4sx) {
	
	double[] p=new double[3];
	
	p[0]=0.0;
	p[1]=0.0;
	p[2]=0.0;
	
	return getSurfCode_Part1(d, antialiasing,
				 "rot_x=0.0; rot_y=0.0; rot_z=0.0;\n", null, null, jv4sx,p);// default;
    }
    
    public String getSurfCode_Part1(Dimension d, boolean antialiasing, 
				    String[] runningParams, 
				    double[] runningParamsValues, 
				    jv4surfex jv4sx) {
	
	double[] p=new double[3];
	
	p[0]=0.0;
	p[1]=0.0;
	p[2]=0.0;
	return getSurfCode_Part1(d, antialiasing,
				 "rot_x=0.0; rot_y=0.0; rot_z=0.0;\n", 
				 runningParams,  
				 runningParamsValues, 
				 jv4sx,
				 p);// default;
    }
    
    public String getSurfCode_Part1(Dimension d, boolean antialiasing,
				    String rotateString, 
				    String[] runningParams, 
				    double[] runningParamsValues, 
				    jv4surfex jv4sx, 
				    double[] zusatzRot) {
	// width and height of the image
	//  System.out.println("getSurfCode_Part1");
	//  d.width = d.width * 80 / 100;
	//  d.height = d.width;
	if (d.width % 2 == 1) {
	    d.width--;
	}
	if (d.height % 2 == 1) {
	    d.height--;
	}
	String strSurf = "";
//	strSurf += "root_finder = d_chain_bisection; epsilon = 0.000000000000001;\n";
	strSurf += "root_finder = "+project.getRootFinder()+"; epsilon = "+
	    project.getEpsilon()+";\n";
	strSurf += "illumination = ambient_light + diffuse_light + reflected_light + transmitted_light;\n";
	strSurf += "ambient      = 40;\ndiffuse      = 80;\nreflected    = 80;\ntransmitted  = 20;\n";
	strSurf += "smoothness   = 50;\ntransparence = 0;\n";
	strSurf += "thickness = 10;\n";
	if (antialiasing) {
	    strSurf += "antialiasing = 6;\n";
	} else {
	    strSurf += "antialiasing = 0;\n";
	}
	strSurf += "background_red="
	    + (project.bgColorButton.getBackground().getRed()) + ";\n"
	    + "background_green="
	    + (project.bgColorButton.getBackground().getGreen()) + ";\n"
	    + "background_blue="
	    + (project.bgColorButton.getBackground().getBlue()) + ";\n";
	//     strSurf += "surface_red=" + (colorButton_o.getBackground().getRed())
	//         + "; surface_green="
	//         + (colorButton_o.getBackground().getGreen())
	//         + "; surface_blue=" + (colorButton_o.getBackground().getBlue())
	//         + ";\n";
	//     strSurf += "inside_red=" + (colorButton_i.getBackground().getRed())
	//         + "; inside_green="
	//         + (colorButton_i.getBackground().getGreen()) + "; inside_blue="
	//         + (colorButton_i.getBackground().getBlue()) + ";\n";
	//  strSurf += "poly w = " + strW + ";\n";
	//  strSurf += "int i = " + strI + ";\n";
	//  strSurf += strPreamble + "\n";
	//    strSurf += "surface=" + text.getText() + ";\n";
	//  strSurf += strMidamble + "\n";
	if (0 == 1) {
	    strSurf += "width=120;\n"+"height=120;\n";
	} else {
	    strSurf += "width=" + d.width + ";\n"+"height=" + d.height + ";\n";
	}
	//
	// rotate, scale, translate
	//
	//    System.out.println("x:"+scale[0]+",y:"+scale[1]+",z:"+scale[2]);
	//    strSurf += "rot_x="+ang[1]+"; rot_y="+ang[0]+"; rot_z="+ang[2]+";\n";
	strSurf += rotateString;//"rot_x=0.0; rot_y=0.0; rot_z=0.0;\n";
	strSurf += "scale_x=" + scale[0] + ";\n"+ 
	    "scale_y=" + scale[1]
	    + ";\n"+ 
	    "scale_z=" + scale[2] + ";\n";
	
	// tetrahedral coordinates:
// 	strSurf += "poly xx=z-1+sqrt(2)*x;\n"+
// 	    "poly yy=z-1-sqrt(2)*x;\n"+
// 	    "poly zz=-(z+1+sqrt(2)*y);\n"+ 
// 	    "poly ww=-(z+1-sqrt(2)*y);\n";
	
	// define some constants:
	strSurf += "double PI = 2*arcsin(1);";
	
	ListIterator li;
	
	// copy the parameters here:
	strSurf += "\n" + "// the parameters:\n";
	int lp = 0;
	int cp = 1;
	li = project.parAdmin.parList.listIterator();
	OneParameter thisop;
	//  System.out.println("pars...");
	String thisSurfCode;
	while (li.hasNext()) {
	    //      System.out.println("no:"+cp);
	    thisop = (OneParameter) li.next();
	    //      if (thisop.isSelected()) {
	    thisSurfCode=thisop.getSurfCode();
	    //System.out.println("was rauskomt:"+thisSurfCode);
	    // falls der Parameter im Film nicht konstant sein sondern laufen soll:
	    if(runningParams!=null){
		for(int kk=0;kk<runningParams.length;kk++)
		    if(thisop.nameLabel.getText().compareTo(runningParams[kk])==0){
			// parameter laeuft gerade
			thisSurfCode=thisop.getSurfCode(runningParamsValues[kk])+"\n";
		    }
	    }
//			System.out.println("was rauskomt:"+thisSurfCode);
	    
	    strSurf += thisSurfCode;
	    lp++;
	    //      }
	    cp++;
	}
	
	// get the equations of the surfaces used:
	strSurf += "\n" + "// the equations:\n";
	li = eqnList.listIterator();
	Equation thisEqn;
	int lc = 0;
	int ec = 1;
	while (li.hasNext()) {
	    thisEqn = (Equation) li.next();
	    strSurf += thisEqn.getPolyCode("f" + ec)+"\n";
	    ec++;
	}

	// the surfaces which will be shown:
	while (li.hasPrevious()) {
	    li.previous();
	}
	lc = 0;
	ec = 1;
//	System.out.println("getSurfCode");
	while (li.hasNext()) {
	    thisEqn = (Equation) li.next();
	    thisEqn.surf_no = -1;
	    if (thisEqn.isSelected() && !(thisEqn.visType.getSelectedIndex()==2)) {
		if (lc <= 9) {
		    // at most 10 surfaces are allowed!
		    strSurf += thisEqn.getSurfCode("f" + ec, lc + 1, 
						   jv4sx.getCameraRotationYXZ())+"\n";
		    lc++;
		    thisEqn.surf_no = lc;
		}
	    }
	    ec++;
	}

	// the clip surfaces:
	while (li.hasPrevious()) {
	    li.previous();
	}
	int lcc = 0;
	int ecc = 1;
	String strSurfNo = "";
	while (li.hasNext()) {
	    thisEqn = (Equation) li.next();
//	    thisEqn.surf_no = -1;
	    if (thisEqn.isSelected() && thisEqn.visType.getSelectedIndex()==2) {
		if (lcc <= 9) {
		    if (lcc >= 1) {
			strSurfNo = "" + (lcc + 1);
		    } else {
			strSurfNo = "";
		    }
		    // at most 10 clip-surfaces are allowed!
		    strSurf += "clip_surface"+strSurfNo+" = f" + ecc+";\n";
		    lcc++;
		    thisEqn.surf_no = lcc;
		}
	    }
	    ecc++;
	}
	
	// get the coordinates of the isolated points 
	// and draw the small spheres:
	strSurf += "\n" + "// the solitary points (shown as spheres):\n";
	li = project.solPtsAdm.solPtsList.listIterator();
	SolitaryPoint thisPt;
	int ptc = 0;
	String strSurface = "";
	Equation tmpEq;
	String strTmp;
	int usedEqNo = lc;
	while (li.hasNext()) {
	    thisPt = (SolitaryPoint) li.next();
	    //      System.out.println("rad:");
	    if (!(((thisPt.radiusLabel).getText()).equals("0.0"))) {
		strSurf += thisPt.getSurfCode();
		ptc++;
		strTmp = thisPt.getSurfNo();
		if (strTmp != "-") {
		    tmpEq = getEqnFromNo(Integer.parseInt(strTmp));
		    if (!(tmpEq == null)) {
			if (tmpEq.surf_no != -1) {
			    // -1 means: do not display
			    if (tmpEq.surf_no == 1) {
				strSurface = "surface";
			    } else {
				strSurface = "surface" + (tmpEq.surf_no);
			    }
			    strSurf += strSurface + " = " + strSurface + "*"
				+ thisPt.getName() + ";\n";
			}
		    }
		}
	    }
	}
	
	// set the rotation:
	strSurf += "\n" + "// the rotation:\n";
	double ang[] = jv4sx.getCameraRotationYXZ();
	for (int i = 0; i < lc; i++) {
	    if (i == 0) {
		strSurface = "surface";
	    } else {
		strSurface = "surface" + (i + 1);
	    }
	    
	    strSurf += strSurface + "=rotate(" + strSurface + "," + (zusatzRot[0])
		+ ",yAxis);\n";
	    strSurf += strSurface + "=rotate(" + strSurface + "," + (zusatzRot[1])
		+ ",xAxis);\n";
	    strSurf += strSurface + "=rotate(" + strSurface + "," + (zusatzRot[2])
		+ ",zAxis);\n";
	    
	    
	    strSurf += strSurface + "=rotate(" + strSurface + "," + (ang[0])
		+ ",yAxis);\n";
	    strSurf += strSurface + "=rotate(" + strSurface + "," + (ang[1])
		+ ",xAxis);\n";
	    strSurf += strSurface + "=rotate(" + strSurface + "," + (ang[2])
		+ ",zAxis);\n";
	    
	    //			System.out.println("0:"+ang[0]+" zusatz:"+zusatzRot[0]);
	    if(zusatzRot[0]!=0){
		//	System.out.println("1:"+ang[1]+" zusatz:"+zusatzRot[1]);
	    }
	    //	System.out.println("2:"+ang[2]+" zusatz:"+zusatzRot[2]);
	}

	// clipping
	strSurf += "clip = "+project.getClipMode()+";\n";
	if(project.getClipMode() == 7) { // i.e. user defined
	    strSurf += "clip_front = 1000; clip_back = -1000;\n";
	}
	strSurf += "radius = "+project.getClipRadius()+";\n";
	
	// set the rotation:
	strSurf += "\n" + "// the rotation:\n";
	for (int i = 0; i < lcc; i++) {
	    if (i == 0) {
		strSurface = "clip_surface";
	    } else {
		strSurface = "clip_surface" + (i + 1);
	    }
	    
	    strSurf += strSurface + "=rotate(" + strSurface + "," + (zusatzRot[0])
		+ ",yAxis);\n";
	    strSurf += strSurface + "=rotate(" + strSurface + "," + (zusatzRot[1])
		+ ",xAxis);\n";
	    strSurf += strSurface + "=rotate(" + strSurface + "," + (zusatzRot[2])
		+ ",zAxis);\n";
	    
	    
	    strSurf += strSurface + "=rotate(" + strSurface + "," + (ang[0])
		+ ",yAxis);\n";
	    strSurf += strSurface + "=rotate(" + strSurface + "," + (ang[1])
		+ ",xAxis);\n";
	    strSurf += strSurface + "=rotate(" + strSurface + "," + (ang[2])
		+ ",zAxis);\n";
	    
	    //			System.out.println("0:"+ang[0]+" zusatz:"+zusatzRot[0]);
	    if(zusatzRot[0]!=0){
		//	System.out.println("1:"+ang[1]+" zusatz:"+zusatzRot[1]);
	    }
	    //	System.out.println("2:"+ang[2]+" zusatz:"+zusatzRot[2]);
	}

						   
	
	//  strSurf += "origin_x=" + origin[0] + "; origin_y=" + origin[1] +
	//      "; origin_z=" + origin[2] + ";\n";
	//  strSurf += "clip = cube; radius=3.2;\n";
	//
	strSurf += "draw_surface;\n";
	//  strSurf += strMidamble2 + "\n";
	
	// draw the curves given by intersections of some of the equations above:
	strSurf += "\n" + "// the curves:\n";
	li = project.cuAdm.cunList.listIterator();
	Curve thisCun;
	lc = 0;
	int cc = 1;
	String tmpSurfNo = "-";
	int iTmpSurfNo = -1;
//	System.out.println("curves...");
	while (li.hasNext()) {
	    thisCun = (Curve) li.next();
//	    System.out.println("next...");
	    if (thisCun.isSelected()) {
//		System.out.println("selected...");
		//    if (lc <= 9) {
		//    System.out.println("cu:"+thisCun.C[0].getSelectedItem().toString());
		tmpSurfNo = (thisCun.C[0].getSelectedItem().toString());
		if (!tmpSurfNo.equals("-")) {
		    iTmpSurfNo = ((Equation) 
				  (eqnList.elementAt(
				      Integer.parseInt(tmpSurfNo) - 1))).surf_no;
//		    System.out.println("surf_no:"+iTmpSurfNo);
		    if (!(iTmpSurfNo == -1)) {
			strSurf += thisCun.getSurfCode(
			    "c" + cc, iTmpSurfNo,
			    jv4sx.getCameraRotationYXZ());
			lc++;
		    }
		}
		//    }
	    }
	    cc++;
	}
	
	strSurf += "\n" + "// save the image:\n";
	strSurf += "filename=\"";
	
	return strSurf;
    }
    
    public String getSurfCode_Part2(String filename) {
	String strSurf = "";
	strSurf += "color_file_format=" + fileFormatFromExt(filename) + ";\n";
//	strSurf += "jpeg_quality=100;";
	strSurf += "save_color_image;\n";
	
	return strSurf;
    }
    
    public String getSurfCode_Dither_Part1(int res) {
	String strSurf = "";
	strSurf += "resolution = " + res + ";\n";
	strSurf += "dither_surface;\n";
	strSurf += "filename=\"";
	
	return strSurf;
    }
    
    public String getSurfCode_Dither_Part2() {
	String strSurf = "\";\n";
	strSurf += "dither_file_format=tiff;\n";
	strSurf += "save_dithered_image;\n";
	
	return strSurf;
    }
    
    public void saveSurfCode(String filename, String imgFilename,
			     boolean antialiasing, jv4surfex jv4sx) {
//	System.out.println("save...");
	try {
	    FileOutputStream fo = new FileOutputStream(filename);
	    
	    PrintWriter pw = new PrintWriter(fo, true);
	    
	    pw.println("scale_x = " + scale[0] + ";");
	    pw.println("scale_y = " + scale[1] + ";");
	    pw.println("scale_z = " + scale[2] + ";");
	    pw.println("radius="+project.getClipRadius()+";");
	    
	    pw.println("illumination = ambient_light + diffuse_light + reflected_light + transmitted_light;");
	    pw.println("ambient      = 40;\ndiffuse      = 80;\nreflected    = 80;\ntransmitted  = 20;");
	    pw.println("smoothness   = 50;\ntransparence = 0;");
	    pw.println("thickness = 10;");
	    if (antialiasing) {
		pw.println("antialiasing = 6;\n");
	    } else {
		pw.println("antialiasing = 0;\n");
	    }
	    pw.println("background_red="
			 + (project.bgColorButton.getBackground().getRed())
			 + "; "
			 + "background_green="
			 + (project.bgColorButton.getBackground().getGreen())
			 + "; " + "background_blue="
			 + (project.bgColorButton.getBackground().getBlue())
			 + ";\n");
	    // Polynome aus der Liste uebergebn
	    ListIterator li = eqnList.listIterator();
	    Equation thisEqn;
	    int lc = 0;
	    int ec = 1;
//	    System.out.println("saveSurfCode");
	    while (li.hasNext()) {
		thisEqn = (Equation) li.next();
		if (thisEqn.isSelected()) {
		    if (lc <= 9) {
			thisEqn.savesurfCode(pw, 
					     "f" + ec, 
					     lc + 1, 
					     jv4sx.getCameraRotationYXZ());
			lc++;
			pw.println("");
		    }
		}
		ec++;
	    }
	    
	    pw.println("draw_surface;");
	    pw.println("filename=\"" + imgFilename + "\";");
	    pw.println("color_file_format=" + fileFormatFromExt(imgFilename)
		       + ";");
//	    pw.println("jpeg_quality=100;");
	    pw.println("save_color_image;");
	    
	    pw.close();
	} catch (IOException er) {
	    System.out.println(er);
	}
    }
    
    public void updateEquationPanel() {
	eqnpanel.removeAll();
	eqnpanel.setLayout(new GridLayout(20, 0));
	//  System.out.println("updatepanel");
	ListIterator li = eqnList.listIterator();
	Equation eq;
	int internalEqnr = 1;
	while (li.hasNext()) {
	    eq = (Equation) li.next();
	    eq.updateActionCommands(internalEqnr++);
	    eqnpanel.add(eq);
	}
	SwingUtilities.updateComponentTreeUI(this);
    }
    
    // Ende Ereignismethoden
    
    // Methoden
    public double[] getAngles() {
	ang = jv4sx.getCameraRotationYXZ();
	return (ang);
    }
    
    public double[] getScales() {
	return (scale);
    }
    
    public String vec2Str(double[] v) {
	return ("<" + v[0] + "," + v[1] + "," + v[2] + ">");
    }
    
    public void newEquation() {
	Equation eqn = new Equation(eqnList.size() + 1, ++eqncounter, this,
				    jv4sx, surfex_);
	eqn.colorButton_i.setBackground(project.appearanceScheme
					.getEquationColorInside(eqn.getEqNo()));
	eqn.colorButton_o.setBackground(project.appearanceScheme
					.getEquationColorOutside(eqn.getEqNo()));
	
	eqnpanel.add(eqn);
	eqnList.add(eqn);
	//tbuttons.add(eqn.basePlaneButton);
	//    tbuttons.add(eqn.deletePlaneButton);
	eqn.basePlaneButton.addActionListener(this);
	eqn.deletePlaneButton.addActionListener(this);
	SwingUtilities.updateComponentTreeUI(this);
    }

    public void loadGeneralData(BufferedReader bs, String datatype,
				int iVersion) {
	try {
	    String tmpStr;
	    String nix = bs.readLine();
	    //      System.out.println("gendata:"+nix);
	    if (datatype.equals("complete") || datatype.equals("onlystyle")) {
		lastUpVector = new PdVector(Double.parseDouble(bs.readLine()),
					    Double.parseDouble(bs.readLine()), Double
					    .parseDouble(bs.readLine()));
//		System.out.println("lup:"+lastUpVector);
		lastViewDir = new PdVector(Double.parseDouble(bs.readLine()),
					   Double.parseDouble(bs.readLine()), Double
					   .parseDouble(bs.readLine()));
		lastPosition = new PdVector(Double.parseDouble(bs.readLine()),
					    Double.parseDouble(bs.readLine()), Double
					    .parseDouble(bs.readLine()));
		lastInterest = new PdVector(Double.parseDouble(bs.readLine()),
					    Double.parseDouble(bs.readLine()), Double
					    .parseDouble(bs.readLine()));
		jv4sx.setScale(Double.parseDouble(bs.readLine()));
		project.setScale(jv4sx.getScale());
		tmpStr = bs.readLine();
		int cTmp = 0;
		try{
		    // test if the file contains a min/max info for the scale
		    cTmp = Integer.parseInt(tmpStr);
//		    System.out.println("int:"+cTmp);
		} catch(Exception intEx) {
//		    System.out.println("no int:"+tmpStr);
		    double tmpD = project.getScale();
		    try{
			jv4sx.setScaleMinMax(Double.parseDouble(tmpStr), 
					     Double.parseDouble(bs.readLine()));
			cTmp = Integer.parseInt(bs.readLine());
		    } catch(Exception doubEx) {
			System.out.println("ex:"+doubEx.toString());
		    }
		}
// 				System.out.println("scale:"+jv4sx.getScale());
// 				System.out.println("scale:"+project.getScale());
//				lastScale = Double.parseDouble(bs.readLine());
		//	project.scaleSlider.setValue((int) (lastScale * 100));
//				project.scaleSliderUpdated();
		project.bgColorButton.setBackground(
		    new Color(cTmp, 
			      Integer.parseInt(bs.readLine()), 
			      Integer.parseInt(bs.readLine())));
		project.antialiasing.setSelected(
		    (Boolean.valueOf(bs.readLine())).booleanValue());
		if(iVersion>=8907) {
		    project.clipMode.setSelectedIndex(
			Integer.parseInt(bs.readLine()));
		    project.clipRadius.setText(bs.readLine());
		}
	    } else {
		if (datatype.equals("specify")) {
		    String strline;
		    System.out.println("load Data");
		    try {
			strline = bs.readLine();
			while ((strline.equals("view:"))
			       || (strline.equals("scale:"))
			       || (strline.equals("background:"))
			       || (strline.equals("antialiasing:"))
			       || (strline.equals("clipMode:"))
			       || (strline.equals("clipRadius:"))) {
			    //          System.out.println("strline:"+strline);
			    if (strline.equals("view:")) {
				lastUpVector = new PdVector(Double
							    .parseDouble(bs.readLine()), Double
							    .parseDouble(bs.readLine()), Double
							    .parseDouble(bs.readLine()));
				lastViewDir = new PdVector(Double
							   .parseDouble(bs.readLine()), Double
							   .parseDouble(bs.readLine()), Double
							   .parseDouble(bs.readLine()));
				lastPosition = new PdVector(Double
							    .parseDouble(bs.readLine()), Double
							    .parseDouble(bs.readLine()), Double
							    .parseDouble(bs.readLine()));
				lastInterest = new PdVector(Double
							    .parseDouble(bs.readLine()), Double
							    .parseDouble(bs.readLine()), Double
							    .parseDouble(bs.readLine()));
			    }
			    if (strline.equals("scale:")) {
				lastScale = Double.parseDouble(bs.readLine());
				jv4sx.setScale(lastScale);
				project.setScale(jv4sx.getScale());
				//				project.scaleSlider
				//					.setValue((int) (lastScale * 100));
				//		project.scaleSliderUpdated();
			    }
			    if (strline.equals("background:")) {
				project.bgColorButton.setBackground(new Color(
									Integer.parseInt(bs.readLine()),
									Integer.parseInt(bs.readLine()),
									Integer.parseInt(bs.readLine())));
			    }
			    if (strline.equals("antialiasing:")) {
				project.antialiasing
				    .setSelected((Boolean.valueOf(bs
								  .readLine())).booleanValue());
			    }
			    if (strline.equals("clipMode:")) {
				project.clipMode.setSelectedIndex(
				    Integer.parseInt(bs.readLine()));
			    }
			    if (strline.equals("clipRadius:")) {
				project.clipRadius.setText(bs.readLine());
			    }
			    bs.mark(255);
			    strline = bs.readLine();
			} // end while
			bs.reset();
		    } catch (Exception nex) {
			bs.reset();
		    }
		} // end if("specify")
	    }
	} catch (Exception exrl) {
	    System.out.println("ex_lg:"+exrl.toString());
	}
	updateJV4SXandReconstructLastView();
    } // end loadGeneralData()
    
    ///
    public void loadEquations(BufferedReader bs, String datatype,
			      int iVersion) {
	//  System.out.println("lE...");
	int k;
	try {
	    String nix = bs.readLine();
	    String num_eqns = bs.readLine();
	    //      System.out.println("num:"+num_eqns);
	    //      System.out.println("nix:"+nix);
	    String zeile1;
	    boolean first = true;
	    
	    for (k = Integer.parseInt(num_eqns); k > 0; k--) {
		zeile1 = bs.readLine();//) != null) {
		if (first) {
		    // lasse erstes element ueberschreiben
		    first = false;
		} else {
		    newEquation();
		}
		if (datatype.equals("specify")) {
		    String strline;
		    try {
			bs.mark(255);
			strline = bs.readLine();
			while ((strline.equals("eqno:"))
			       || (strline.equals("equation:"))
			       || (strline.equals("insidecolor:"))
			       || (strline.equals("outsidecolor:"))
			       || (strline.equals("basecolor:"))
			       || (strline.equals("showcbox:"))
			       || (strline.equals("clipping:"))
			       || (strline.equals("clipradius:"))
			       || (strline.equals("transparency:"))) {
			    //          System.out.println("strline:"+strline);
			    if (strline.equals("eqno:")) {
				((Equation) eqnList.lastElement())
				    .setEquationNo(Integer.parseInt(bs
								    .readLine()));
			    }
			    if (strline.equals("equation:")) {
				((Equation) eqnList.lastElement()).text
				    .setText(bs.readLine());
			    }
			    if (strline.equals("insidecolor:")) {
				((Equation) eqnList.lastElement()).colorButton_i
				    .setBackground(new Color(
						       Integer.parseInt(bs.readLine()),
						       Integer.parseInt(bs.readLine()),
						       Integer.parseInt(bs.readLine())));
			    }
			    if (strline.equals("outsidecolor:")) {
				((Equation) eqnList.lastElement()).colorButton_o
				    .setBackground(new Color(
						       Integer.parseInt(bs.readLine()),
						       Integer.parseInt(bs.readLine()),
						       Integer.parseInt(bs.readLine())));
			    }
			    if (strline.equals("basecolor:")) {
				int i = Integer.parseInt(bs.readLine());
				if (i != 255) {
				    ((Equation) eqnList.lastElement()).basePlaneButton
					.setBackground(new Color(0, 255, 0));
				} else {
				    ((Equation) eqnList.lastElement()).basePlaneButton
					.setBackground(new Color(255, 255,
								 255));
				}
			    }
			    if (strline.equals("showcbox:")) {
				((Equation) eqnList.lastElement()).cbox
				    .setSelected((Boolean.valueOf(bs
								  .readLine())).booleanValue());
			    }
			    if (strline.equals("clipping:")) {
				((Equation) eqnList.lastElement()).optionButtonPane.bt2
				    .setSelected(!(Boolean.valueOf(bs
								   .readLine()).booleanValue()));
			    }
			    if (strline.equals("clipradius:")) {
				((Equation) eqnList.lastElement()).optionButtonPane.clipingRadiusTextField
				    .setText(bs.readLine());
			    }
			    if (strline.equals("transparency:")) {
				((Equation) eqnList.lastElement()).optionButtonPane.slider1
				    .setValue(Integer.parseInt(bs
							       .readLine()));
			    }
			    bs.mark(255);
			    strline = bs.readLine();
			} // end while
			bs.reset();
		    } catch (Exception nex) {
			System.out.println("lE ex:" + nex);
			bs.reset();
		    }
		} else {
		    if (datatype.equals("complete")
			|| datatype.equals("onlyeqns")
			|| datatype.equals("eqnsvis")) {
			//      System.out.println("eqno, equations...");
			((Equation) eqnList.lastElement())
			    .setEquationNo(Integer.parseInt(bs.readLine()));
			((Equation) eqnList.lastElement()).text.setText(bs.readLine());
			//      System.out.println("end eqno, equations.");
		    }
		    if (datatype.equals("complete")
			|| datatype.equals("onlystyle")) {
			//      System.out.println("colors...");
			((Equation) eqnList.lastElement()).colorButton_i
			    .setBackground(new Color(
					       Integer.parseInt(bs.readLine()), 
					       Integer.parseInt(bs.readLine()), 
					       Integer.parseInt(bs.readLine())));
			((Equation) eqnList.lastElement()).colorButton_o
			    .setBackground(new Color(Integer.parseInt(bs.readLine()), 
						     Integer.parseInt(bs.readLine()), 
						     Integer.parseInt(bs.readLine())));
			int i = Integer.parseInt(bs.readLine());
			if (i != 255) {
			    //      System.out.println("green");
			    ((Equation) eqnList.lastElement()).basePlaneButton
				.setBackground(new Color(0, 255, 0));
			} else {
			    //      System.out.println("white");
			    ((Equation) eqnList.lastElement()).basePlaneButton
				.setBackground(new Color(255, 255, 255));
			}
		    }
		    
		    if (datatype.equals("complete")
			|| datatype.equals("onlystyle")
			|| datatype.equals("eqnsvis")) {
			//      System.out.println("cbox...");
			((Equation) eqnList.lastElement()).cbox
			    .setSelected((Boolean.valueOf(bs.readLine()))
					 .booleanValue());
			if(iVersion>=8907) {
			    ((Equation) eqnList.lastElement()).visType
				.setSelectedIndex(Integer.parseInt(bs.readLine()));
			}
		    }
		    if (datatype.equals("complete")
			|| datatype.equals("onlystyle")) {
			((Equation) eqnList.lastElement()).optionButtonPane.bt2
			    .setSelected(!(Boolean.valueOf(bs.readLine())
					   .booleanValue()));
			((Equation) eqnList.lastElement()).optionButtonPane
			    .clipingRadiusTextField
			    .setText(bs.readLine());
		    }
		    if (datatype.equals("complete")
			|| datatype.equals("onlystyle")
			|| datatype.equals("eqnsvis")) {
			//      System.out.println("slider1...");
			((Equation) eqnList.lastElement()).optionButtonPane.slider1
			    .setValue(Integer.parseInt(bs.readLine()));
		    }
		}
	    }
	} catch (IOException e) {
	    System.out.println(e);
	}
	//  System.out.println("end lE.");
    } // end loadEquations()
    
    public void save(PrintWriter pw) {
	//  System.out.println("save Eq" );
	ListIterator li = eqnList.listIterator();
	pw.println("////////////////// EQUATIONS: /////////////////////////");
	pw.println(eqnList.size());
	while (li.hasNext()) {
	    ((Equation) li.next()).saveYourself(pw);
	}
    }
    
    public String save() {
	String str = "";
	//  System.out.println("save Eq" );
	ListIterator li = eqnList.listIterator();
	str += "////////////////// EQUATIONS: /////////////////////////" + "\n";
	str += eqnList.size() + "\n";
	while (li.hasNext()) {
	    str += ((Equation) li.next()).saveYourself();
	}
	return (str);
    }
    
    public void updateJV4SX() {
	////////////////////
	/// muss noch gefuellt werden ...
	// wird aufgeufen, wenn das Projekt gewechselt wurde
	// um das Bildchen in javaview upzudaten
	ListIterator li = eqnList.listIterator();
	Equation eq;
	while (li.hasNext()) {
	    eq = (Equation) li.next();
	    if (eq.isBasePlane()) {
		eq.updateJV4SX();
		break;
	    }
	}
	
	jv4sx.changeFrameTitle(projectName
			       + "triangulated view - surfex (uses JavaView)");
    }
    
    public void updateJV4SXandReconstructLastView() {
//	System.out.println("updateJV4SXandRec...");
	ListIterator li = eqnList.listIterator();
	Equation eq;
	while (li.hasNext()) {
	    eq = (Equation) li.next();
	    //      System.out.println("is base?"+eq.getEqNo());
	    if (eq.isBasePlane()) {
		eq.updateJV4SX();
		break;
	    }
	}
	
	//		System.out.println("scale2:"+jv4sx.getScale());
	//  jv4sx.disp.getCamera().setUpVector(lastUpVector);
	// jv4sx.disp.getCamera().setViewDir(lastViewDir);
	jv4sx.disp.getCamera().setInterest(lastInterest);
	jv4sx.disp.getCamera().setPosition(lastPosition);
//	System.out.println("lup:"+lastUpVector);
	jv4sx.disp.getCamera().setUpVector(lastUpVector);
//		jv4sx.disp.fit();
//	System.out.println("scale EA:"+project.getScale());
	jv4sx.setScale(project.getScale());
	//		jv4sx.disp.getCamera().setScale(lastScale);
	//jv4sx.disp.getCamera().setFullPosition(lastInterest, lastPosition, lastUpVector);
//	System.out.println("scale3:"+jv4sx.getScale());
	
	jv4sx.changeFrameTitle(projectName
			       + "triangulated view - surfex (uses JavaView)");
	//System.out.println("u"+vec2Str(jv4sx.getCamPos()));
    }
} // end class Equation Admin

