////////////////////////////////////////////////////////////////////////
//
// This file jv4surfex.java is part of SURFEX.
//
////////////////////////////////////////////////////////////////////////

//
// Parts of this file were written by Eike Preuss (a javaview developer)
//
/** 
 * Load a file, display, on request return rotation.
 * 
 * @see     jv.viewer.PvViewer
 * @author    Eike Preuss
 * @version   17.10.01, 1.00 created (ep) copied from javaview.java :-)<br>
 */ 

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

import java.applet.AppletContext;
import java.applet.Applet;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;


import java.awt.*;
import java.util.*;//Hashtable;

import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JSlider;
import javax.swing.JTextField;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
//import javax.swing.JFrame;

import jv.geom.PgElementSet;

import jv.object.PsConfig;
import jv.project.PvDisplayIf;
import jv.project.PgGeometry;
import jv.viewer.PvDisplay;
import jv.viewer.PvViewer;
import jv.loader.PjImportModel;
import jv.loader.PgJvxLoader;
import jv.loader.PgLoader;
import jv.project.PvCameraIf;
import jv.vecmath.PdVector;

public class jv4surfex {
    private boolean inAnApplet = false;
    
    double lastScale,factor;
    
    Vector lamps=new Vector();
    
    double scale = 1.0;
    public double scaleMin = 0.0;
    public double scaleMax = 10.0;
    JSlider scaleSlider = null;
    JLabel scaleLabel = null;
    JTextField scaleLabelMin = null;
    JTextField scaleLabelMax = null;
    JTextField scaleLabelCur = null;
    
    LampAdminUpdater lampAdminUpdater;//=new LampAdminUpdater();
    
    private Frame frame = null;

    Panel pScale = null;    
    //    public PvDisplayIf disp = null;
    
    int xPos = 824;
    
    boolean lastb=false;
    
    Frame jv4sxFrame;
    
    public void setScaleMinMax(double smin, double smax) {
	scaleMin = smin;
	scaleMax = smax;
	lampAdminUpdater.setScaleMinMax(scaleMin, scaleMax);
	scaleSlider.setValue((int)((scale-scaleMin)/(scaleMax-scaleMin)*1000+0.5));
	scaleLabelMin.setText(""+((int)(1+scaleMin*1000))/1000.0);
	scaleLabelMax.setText(""+((int)(scaleMax*1000))/1000.0);
	pScale.repaint();
    }

    public void setScale(double s) {
//	System.out.println("setScale jv4:"+s);
	scale = s; 
//	System.out.println("scale1:"+scale);
	try {
	    surfex_.getCurrentProject().setScale(scale);
	} catch(Exception e) {
	}
	disp.getCamera().setScale(scale);
	//	System.out.println("scale:"+scaleSlider.getValue());
//	scaleSlider.setValue((int)(scale*100+0.5));
	scaleSlider.setValue((int)((scale-scaleMin)/(scaleMax-scaleMin)*1000+0.5));
//	scaleLabelCur.setText(""+scaleSlider.getValue()/100.0);
//	System.out.println("slider:"+(int)((scale-scaleMin)/(scaleMax-scaleMin)*1000+0.5)+","+scaleSlider.getValue()+", min:"+scaleSlider.getMinimum()+", max:"+scaleSlider.getMaximum());
	scaleLabelCur.setText(""+(scaleMin+(scaleSlider.getValue())/1000.0*(scaleMax-scaleMin)));
//	System.out.println("scale2:"+scaleLabelCur.getText()+","+scale);
	updateDisp();
    }
    
    public double getScale() {
//	System.out.println("scale:"+scale);
	return(scale);
    }
    
    public void setVisible(boolean b){
	frame.setVisible(b);
    }
    
    public PvDisplay disp = null;
    
    surfex surfex_;
    
    public jv4surfex(boolean iaa, Applet theApplet, surfex sx, 
		     int xPos, JTextField x,JTextField y,JTextField z) {
	this.xPos = xPos;
	init(iaa, theApplet, sx,x,y,z,1);
    }
    public jv4surfex(boolean iaa, Applet theApplet, surfex sx, 
		     int xPos, double factor) {
	this.xPos = xPos;
	init(iaa, theApplet, sx, new JTextField(),
	     new JTextField(), new JTextField(),factor);
    }
    
    public jv4surfex(boolean iaa, Applet theApplet, surfex sx,
		     JTextField x,JTextField y,JTextField z) {
	init(iaa, theApplet, sx,x,y,z,1);
    }

    public jv4surfex(boolean iaa, Applet theApplet, surfex sx,
		     JTextField x,JTextField y,JTextField z,
		     double factor) {
	init(iaa, theApplet, sx,x,y,z,factor);
    } 
    
    public void init(boolean iaa, Applet theApplet, surfex sx,
		     JTextField x,JTextField y,JTextField z, double factor){
	inAnApplet = iaa;
	surfex_ = sx;
	lampAdminUpdater=new LampAdminUpdater(factor, scaleMin, scaleMax);
	
	this.factor=factor;
	frame = new Frame("triangulated view - surfex (uses JavaView)");
	frame.setLayout(new BorderLayout());
	frame.setVisible(false);
	// Create viewer for viewing 3d geometries, and register frame.
	//	PvViewer viewer = null;
	//	System.out.println("iaa: "+inAnApplet);
	//	System.out.println("theApplet: "+theApplet);
	if(inAnApplet) {
	    //	    System.out.println("iaa!");
	    //	    viewer = new PvViewer(theApplet, new Frame());
	} else {
	    //	    viewer = new PvViewer(null, new Frame());
	}
	// Get default display from viewer
	//	disp = viewer.getDisplay();
	disp = new PvDisplay();
	PvCameraIf cam = disp.getCamera();
	//    cam.setUpVector(new PdVector(0.0,1.0,0.0));
	//    cam.setViewDir(new PdVector(1.0,2.0,2.0));
	//    cam.setPosition(new PdVector(10.0,20.0,-20.0));
	
	// Add display to frame
	frame.add(BorderLayout.CENTER, (Component) disp);
	
	// the Slider for scaling:
	scaleSlider = new JSlider(1, 1000);
	scale = 1.0;
	scaleLabel = new JLabel("1.0");
	scaleSlider.setValue(100);
	scaleSlider.setMinorTickSpacing(10);
	scaleSlider.setMajorTickSpacing(100);
	scaleSlider.setPaintTicks(true);
// 	Hashtable labelTable = new Hashtable();
// 	labelTable.put(new Integer(1), new JLabel("1"));
// 	labelTable.put(new Integer(500), new JLabel("5"));
// 	labelTable.put(new Integer(1000), new JLabel("10"));
//	scaleSlider.setLabelTable(labelTable);
	
/*	    scaleSlider.addChangeListener(new ChangeListener() {
	    public void stateChanged(ChangeEvent evt) {
	    String str = "" + scaleSlider.getValue();// / 100.0;
	    int end = 6;
	    if (str.length() <= 6) {
	    end = str.length();
	    }
	    scaleLabel.setText(str.substring(0, end));
	    //       scaleSliderUpdated();
	    }
	    });
*/    // we do not show the scaleSlider for the moment,
	// because it does not work yet.
	//lampPanel[5].add(new JLabel("scale:"));
	Panel p2=new Panel(new BorderLayout());
	Panel p3=new Panel(new GridLayout(1,6));
	Panel pScaleVals=new Panel(new BorderLayout());

	scaleLabelMin = new JTextField(""+((int)(1+scaleMin*1000))/1000.0);
	scaleLabelMax = new JTextField(""+((int)(scaleMax*1000))/1000.0);
	scaleLabelCur = new JTextField("0.0");
	scaleLabelMin.setEditable(true);
	scaleLabelMax.setEditable(true);
	scaleLabelCur.setEditable(false);
	scaleLabelMin.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent evt) {
		    try {
			double tmp = Double.parseDouble(scaleLabelMin.getText());
			if (tmp <= scaleMax && tmp <= getScale()) {
			    tmp = Math.round(tmp * 1000) / 1000.0;
			    setScaleMinMax(tmp, scaleMax);
			} else {
			    scaleLabelMin.setText("" + scaleMin);
			}
		    } catch (Exception nanEx) {
			scaleLabelMin.setText("" + scaleMin);
		    }
		}
	    });
	scaleLabelMax.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent evt) {
		    try {
			double tmp = Double.parseDouble(scaleLabelMax.getText());
			if (tmp >= scaleMin && tmp >= getScale()) {
			    tmp = Math.round(tmp * 1000) / 1000.0;
			    setScaleMinMax(scaleMin, tmp);
			} else {
			    scaleLabelMax.setText("" + scaleMax);
			}
		    } catch (Exception nanEx) {
			scaleLabelMax.setText("" + scaleMax);
		    }
		}
	    });
	pScaleVals.add(BorderLayout.WEST, scaleLabelMin);
	pScaleVals.add(BorderLayout.EAST, scaleLabelMax);
	pScaleVals.add(BorderLayout.CENTER, scaleLabelCur);

	pScale = new Panel(new BorderLayout());
	pScale.add(BorderLayout.NORTH, p2);
	pScale.add(BorderLayout.SOUTH, pScaleVals);

	frame.add(BorderLayout.SOUTH, pScale);
	
//	    p2.add(BorderLayout.CENTER,p3);
	//add(scaleLabel);
	p2.add(BorderLayout.SOUTH,scaleSlider);
	p3.add(new JLabel(" x:"));
	p3.add(x);
	p3.add(new JLabel(" y:"));
	p3.add(y);
	p3.add(new JLabel(" z:"));
	p3.add(z);
	
	frame.pack();
	// Position of left upper corner and size of frame when run as application.
	
	frame.setBounds(new Rectangle(xPos, 0, 200, 200));
	
	frame.setVisible(false);
	
	lampAdminUpdater.setJv4sx(this);
	lampAdminUpdater.setSlider(scaleSlider);
	lampAdminUpdater.setPosTextFields(x,y,z);
	lampAdminUpdater.start();
	
    }
    
    public void setPosTextFields2(JTextField x,JTextField y,JTextField z){
	
	lampAdminUpdater.setPosTextFields2(x,y,z);
    }
    public void setPosTextFields(JTextField x,JTextField y,JTextField z){
	
	lampAdminUpdater.setPosTextFields(x,y,z);
    }
    
    /*public void setCamPos(double x, double y, double z){
      lampAdminUpdater.setCamPos(x,y,z);
      }*/
    
    
    
    /*public void updateScaleSliderValue(){
      scaleSlider.setValue((int)disp.getCamera().getScale());
      scaleLabel.setText(Double.toString(disp.getCamera().getScale()));
      }*/
    
    /*public void scaleSliderUpdated() {
    //System.out.println("ssUpd:"+Double.parseDouble(scaleLabel.getText()));
    lastScale = Double.parseDouble(scaleLabel.getText());
    //System.out.println("scale:"+scaleLabel.getText());
    //System.out.println("scale:"+eqAdm.lastScale);
    this.disp.getCamera().setScale(lastScale);
    this.updateDisp();
    System.out.println("testsjfd"+lastScale);
    //surfex_.raytrace();
    //eqAdm.updateJV4SXandReconstructLastView();
    
    }*/
    
    public void changeFrameTitle(String s) {
	frame.setTitle(s);
    }
    
    public void show() {
//	     System.out.println("s44 "+disp.getCamera().getScale());
	
	double lastScale_tmp=disp.getCamera().getScale();
	frame.setVisible(true);
	setScale(scale);
//	     disp.getCamera().setScale(lastScale_tmp);
//	     disp.getCamera().setScale(scale);
//	     System.out.println("s46 "+disp.getCamera().getScale());
//	     updateDisp();
	frame.toFront();
	//	disp.fit();
    }
    
    public void setParameterWarning(boolean b){
	if(b!=lastb){
	    if(b){
		jv4sxFrame=frame;
		frame=new Frame();
		frame.add(new Label("You can use jv4sx only without parameters"));
	    }else{
		frame=jv4sxFrame;
	    }
	}
    }
    
    public void hide() {
	frame.setVisible(false);
    }
    
    public void setLamps(Vector lamps){
	this.lamps=lamps;
//		System.out.println("222joahr----"+((Lamp)lamps.firstElement()).getXpos());
	
    }
    
    public boolean showDefault() {
	//	System.out.println("showDefault()");
	PgElementSet geom = new PgElementSet();
	PgElementSet geom2 = new PgElementSet();
	PgElementSet[] geoma = new PgElementSet[9];
	
	
	geom.setName("Sphere");
	// Compute coordinates and mesh of a geometry. Other tutorials
	// show more details how to create one geometries.
	//geom.setNumElements(lamps.size());
	
	//geom.(new PdVector(100000000,100,100));
	//geom.setCenter(new PdVector(100000000,100,100));
	
	geom.computeSphere(10, 10, factor/3);
	//	geom.setTransparency(1000.5);
	
	disp.removeGeometries();
	
	
	int i=lamps.size()-1;
	Lamp l;
//		System.out.println("lampsize:"+lamps.size());
	for(i=0;i<lamps.size();i++){
	    //	System.out.println("333joahr----"+((Lamp)lamps.firstElement()).getXpos());
	    
	    l=(Lamp)lamps.elementAt(i);//(Lamp)li.next();
	    
	    PdVector[] p=new PdVector[4];
	    
	    //l=;
	    //	System.out.println(i+"joahr");
	    //	System.out.println(l);
	    p[0]=new PdVector(l.getXpos()-1,l.getYpos()+0,l.getZpos()+1);
	    p[1]=new PdVector(l.getXpos()-1,l.getYpos()+0,l.getZpos()-1);
	    p[2]=new PdVector(l.getXpos()-1,l.getYpos()-1,l.getZpos());
	    p[3]=new PdVector(l.getXpos()-1,l.getYpos()+1,l.getZpos());
	    //geom.computeSphere(10, 10, factor/3);
	    
	    /*
	      p[0]=new PdVector(0,0,0);
			p[1]=new PdVector(10000,0,10000);
			p[2]=new PdVector(0,10000,10000);
			p[3]=new PdVector(0,0,10000);
	    */
	    
//			geoma[i].setGeneralElementColor(l.colorButton.getBackground());
	    geoma[i] = new PgElementSet();
	    geoma[i].computeSphere(10, 10, factor/30);
	    
	    
	    
	    
	    geoma[i].translate(new PdVector(l.getXpos(),l.getYpos(),l.getZpos()));//setElementVertices(i,p);
	    disp.addGeometry(geoma[i]);
	    //	disp.fit();
	    //	disp.addGeometry(geom);
	    //		disp.showAxes(true);
	}
	
	
	
	// folgendes wird gemacht, damit die grosse Kugel in der Mitte steht und nicht irgenwas schief geht ...
	// ich habe alles andere versucht und nun nach 12h bug-suche scheint mir dies die letzte Moeglichkeit
	
	
	disp.addGeometry(geom);
	
	addCentr(1000000000,0,0);
	addCentr(-1000000000,0,0);
	addCentr(0,-100000000,0);
	addCentr(0,100000000,0);
	addCentr(0,0,-1000000000);
	addCentr(0,0,1000000000);
	
	disp.fit();
	disp.update(disp);
	return true;
    }
    
    private void addCentr(int x, int y, int z){
	PgElementSet centratoren1 = new PgElementSet();
	centratoren1.computeSphere(2, 2, 1.0);
	centratoren1.translate(new PdVector(x,y,z));
	disp.addGeometry(centratoren1);
	
    }
    
    public void updateDisp() {
	disp.update(disp);
    }
    
    /**
     * @return    false, if file could not be loaded
     */
    public boolean loadAndDisplay(String fullfilename) {
	try {
	    PgJvxLoader myJvxLoader = new PgJvxLoader();
	    //		System.out.println("loader created!");
	    PgLoader myLoader = new PgLoader();
	    //		System.out.println("loader created:"+fullfilename+".");
	    PgGeometry geoms[];
	    if (inAnApplet) {
		geoms = myLoader.load(surfex_.webPrgs.surfPath.getText()
				      + fullfilename);
	    } else {
		geoms = myLoader.load(fullfilename);
	    }
	    
	    //		System.out.println("loaded!");		
	    //		System.out.println("title:"+geoms[0].getTitle());
	    disp.removeGeometries();
	    disp.addGeometry(geoms[0]);
	    //		disp.fit();
	    disp.update(disp); 
	    return (true);
	} catch (Exception e) {
	    System.out.println("PgJvxLoader:" + e.toString());
	}
	return false;
    }
    
    /**
     * Returns angles you have to rotate about y,x and z axis.
     */
    public double[] getViewDir() {
	PvCameraIf cam = disp.getCamera();
	PdVector third = PdVector.copyNew(cam.getViewDir());
	third.normalize();
	return (third.m_data);
    }
    
    public double[] getUpVector() {
	PvCameraIf cam = disp.getCamera();
	PdVector second = PdVector.copyNew(cam.getUpVector());
	second.normalize();
	return (second.m_data);
    }
    
    public double[] getRightVector() {
	PvCameraIf cam = disp.getCamera();
	PdVector third = PdVector.copyNew(cam.getViewDir());
	third.multScalar(-1.);
	third.normalize();
	PdVector second = PdVector.copyNew(cam.getUpVector());
	second.normalize();
	PdVector first = PdVector.crossNew(second, third);
	//  first.multScalar(-1.0);
	first.normalize();
	return (first.m_data);
    }
    
    public double[] getCamPos() {
	PvCameraIf cam = disp.getCamera();
	PdVector pos = PdVector.copyNew(cam.getPosition());
	//	pos.multScalar(2.0);
	return (pos.m_data);
	}
    
    public void setCamPos2(double x,double y,double z) {
	PvCameraIf cam = disp.getCamera();
	cam.setPosition(new PdVector(x,y,z));
	//	pos.multScalar(2.0); 
	//return (pos.m_data);
    }
	/*
	  public double[] getCamPos_withoutFac2() {
	  PvCameraIf cam = disp.getCamera();
	  PdVector pos = PdVector.copyNew(cam.getPosition());
	  //	pos.multScalar(2.0);
	  return (pos.m_data);
	  }*/
    
    public double[] getCameraRotationYXZ() {
	PvCameraIf cam = disp.getCamera();
	PdVector third = PdVector.copyNew(cam.getViewDir());
	//    System.out.println("viewDir:"+third);
	third.multScalar(-1.);
	third.normalize();
	PdVector second = PdVector.copyNew(cam.getUpVector());
	//    System.out.println("upVector:"+second);
	second.normalize();
	PdVector first = PdVector.crossNew(second, third);
	//    System.out.println("crossNew:"+first);
	
	double[] ret = PdVector.frameToStandardFrame(first, second, third);
	//    System.out.println("ret:"+ret[0]+", "+ret[1]+", "+ret[2]);
	ret[0] = -ret[0];
	return ret;
    }
} 
