////////////////////////////////////////////////////////////////////////
//
// This file LampAdminUpdater.java is part of SURFEX.
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

import javax.swing.*;
import java.awt.*;

public class LampAdminUpdater extends Thread {
    
    double lastjv4sxScale = 0;
    
    double[] lastCamPos={0.0,0.0,0.0};
    double[] lastCamRot={0.0,0.0,0.0};
    
    double eps=0.0001;
    
    int zoomFac=1;
    
    String lastx = "0";
    String lasty = "0";
    String lastz = "0";
    String lastx2 = "0";
    String lasty2 = "0";
    String lastz2 = "0";
    
    double lastzoom = 0;
    
    double factor;
    double scaleMin;
    double scaleMax;
    
    jv4surfex jv4sx;
    
    JTextField x=new JTextField("0");
    JTextField y=new JTextField("0");
    JTextField z=new JTextField("0");
    JTextField x2=new JTextField("0");
    JTextField y2=new JTextField("0");
    JTextField z2=new JTextField("0");
    
    JSlider zoom;
    
    int genauigkeit=8;
    
    int timedist = 5; //50ms distanz zwischen updates
    
    LampAdminUpdater() {
	
    }
    
/*	public double[] getScaleForSurf(){
//	return 1.0 / jv4sx.disp.getCamera().getScale()*surffactor;
}
*/
    LampAdminUpdater(double factor, double smin, double smax) {
//	    System.out.println("factor:"+factor);
//	    this.factor=4/factor/1000;
	scaleMin = smin;
	scaleMax = smax;
	this.factor=1/factor*(scaleMax-scaleMin)/1000;
    }
    
    public void setScaleMinMax(double smin, double smax) {
	scaleMin = smin;
	scaleMax = smax;
	this.factor=1/1.0*(smax-smin)/1000;
    }
    
    public synchronized void setSlider(JSlider s) {
	zoom = s;
    }
    
    public synchronized void setPosTextFields(JTextField x, JTextField y, JTextField z) {
	this.x = x;
	this.y = y;
	this.z = z;
	//this.x2.setText(x.getText());
	
	//this.y2.setText(y.getText());
	
	//this.z2.setText(z.getText());
    }
    
    public synchronized void setPosTextFields2(JTextField x, JTextField y, JTextField z) {
	
	//	System.out.println(0+" "+jv4sx.getCamPos()[2]);
	
	
	this.x2 = x;
	this.y2 = y;
	this.z2 = z;
	
	//	System.out.println(0+".5 "+jv4sx.getCamPos()[2]);
	//System.out.println(1+" "+x2.getText());
	//this.x.setText(x.getText());
	
	//this.y.setText(y.getText());
	
	//this.z.setText(z.getText());
	//updateLast();
    }
    
    public synchronized void setJv4sx(jv4surfex j) {
	jv4sx = j;
    }
    
    public void run() {
	while (true) {
	    try {
		sleep(timedist);
	    } catch (Exception e) {
		
	    }
	    // testen ob der Scale geaendert wurde
	    // in jv4sx:
	    /*	if (jv4sx.disp.getCamera().getScale() != lastjv4sxScale) {
		update();
		///	System.out.println("1");
		
		} else {*/
	    //77		if (check(jv4sx.getCameraRotationYXZ(),lastCamRot)) {
	    //	update();
	    //	System.out.println(jv4sx.getCameraRotationYXZ()[0]+" "+lastCamRot[0]);
	    //	System.out.println("2");
	    //	} else {
	    if (check(jv4sx.getCamPos(), lastCamPos)) {
		update();
		//		System.out.println(":3");
	    } else {
		// im Zoom-slider
		if (zoom.getValue() != lastzoom) {
		    updateZoom();
		    //		System.out.println(":4");
		} else {
		    if (x.getText().compareTo(lastx) != 0
			|| y.getText().compareTo(lasty) != 0
			|| z.getText().compareTo(lastz) != 0) {
			updatePos();
			//		System.out.println(":5");
		    }else{
			if (x2.getText().compareTo(lastx2) != 0
			    || y2.getText().compareTo(lasty2) != 0
			    || z2.getText().compareTo(lastz2) != 0) {
			    updatePos2();
			    //		System.out.println(":6");
			}
		    }
		}
		//}
		//}
		
	    }
	    
	}
    }
    
    public synchronized boolean check(double[] a,double[] b){
	for(int i=0;i<a.length;i++){
	    
	    if(a[i]-b[i]>eps){
//				System.out.println("............"+(a[i]-b[i]));
		
		return true;
	    }
	}
	return false;
    }
    
    public synchronized void update() {
	double[] pos=jv4sx.getCamPos();
	if(pos[0]<lastCamPos[0]*100){
	    x.setText(getString((pos[0]+"")));
	    y.setText(getString((pos[1]+"")));
	    z.setText(getString((pos[2]+"")));
	    x2.setText(getString((pos[0]+"")));
	    y2.setText(getString((pos[1]+"")));
	    z2.setText(getString((pos[2]+"")));
	    
//		System.out.println("factor:"+factor);
//	    zoom.setValue((int)(jv4sx.getScale()*zoomFac/factor));
	    zoom.setValue((int)((jv4sx.getScale()-scaleMin)/(scaleMax-scaleMin)*1000+0.5));
	    
	    //	System.out.println(2+" "+x2.getText());
	    updateLast();
	}else{
	    updateZoom();
	}
    }
    
    /*public void setCamPos(double x,double y,double z){
      updateLast();
      }*/
    public synchronized void updateZoom() {
//	jv4sx.setScale((double)zoom.getValue()/zoomFac*factor);
	jv4sx.setScale((scaleMin+(zoom.getValue())/1000.0*(scaleMax-scaleMin)));
	double[] pos=jv4sx.getCamPos();
	x.setText(getString((pos[0]+"")));
	y.setText(getString((pos[1]+"")));
	z.setText(getString((pos[2]+"")));
	x2.setText(getString((pos[0]+"")));
	y2.setText(getString((pos[1]+"")));
	z2.setText(getString((pos[2]+"")));
	
	jv4sx.updateDisp();
	updateLast();
	
	
	//System.out.println(3+" "+x2.getText());
    }
    public synchronized void updatePos() {
	jv4sx.setCamPos2(Double.valueOf(x.getText()+"0").doubleValue(),Double.valueOf(y.getText()+"0").doubleValue(),Double.valueOf(z.getText()+"0").doubleValue());
//	zoom.setValue((int)(jv4sx.getScale()*zoomFac/factor));
	zoom.setValue((int)((jv4sx.getScale()-scaleMin)/(scaleMax-scaleMin)*1000+0.5));
	x2.setText(getString(x.getText()));
	y2.setText(getString(y.getText()));
	z2.setText(getString(z.getText()));
	
	jv4sx.updateDisp();
	
	updateLast();
	
	//System.out.println(4+" "+x2.getText());
    }
    
    public synchronized void updatePos2() {
	jv4sx.setCamPos2(Double.valueOf(x2.getText()+"0").doubleValue(),Double.valueOf(y2.getText()+"0").doubleValue(),Double.valueOf(z2.getText()+"0").doubleValue());
//	zoom.setValue((int)(jv4sx.getScale()*zoomFac/factor));
	zoom.setValue((int)((jv4sx.getScale()-scaleMin)/(scaleMax-scaleMin)*1000+0.5));	x.setText(getString(x2.getText()));
	y.setText(getString(y2.getText()));
	z.setText(getString(z2.getText()));
	
	jv4sx.updateDisp();
	
	updateLast();
	
	//System.out.println(5+".......... "+x2.getText());
    }
    
    public synchronized void updateLast(){
	lastjv4sxScale = jv4sx.getScale();
	
	lastCamPos=jv4sx.getCamPos();
	lastCamRot=jv4sx.getCameraRotationYXZ();
	
	lastx = x.getText();
	
	lasty = y.getText();
	
	lastz = z.getText();
	lastx2 = x2.getText();
	
	lasty2 = y2.getText();
	
	lastz2 = z2.getText();
	
	lastzoom = zoom.getValue();
    }
    
    
    public synchronized String getString(String s){
	return (s+"          ").substring(0,8).replaceAll(" ","");
    }
}
