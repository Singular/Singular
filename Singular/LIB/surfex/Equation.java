////////////////////////////////////////////////////////////////////////
//
// This file Equation.java is part of SURFEX.
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
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileWriter;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.URL;
import java.net.URLConnection;
import java.net.URLEncoder;

import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JColorChooser;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;

//////////////////////////////////////////////////////////////
//
// class Equation
// 
//////////////////////////////////////////////////////////////

public class Equation extends JPanel {

    // Anfang Variablen 
    
    surfex surfex_;
    
    public int surf_no = -1;
    
    // GUI
    public JTextField text;
    
    JPanel configPanel = new JPanel();
    
    JButton basePlaneButton = new JButton();
    
    JButton optionButton = new JButton("opts");
    
    JButton deletePlaneButton = new JButton("del");
    
    JLabel label1, label2;
    
    String eindeutigeEqnNr;
    
    JButton colorButton_i, colorButton_o;
    
//    JPanel labelpanel = new JPanel(new GridLayout(2, 1));
    
    int eqnr;
    
    jv4surfex jv4sx;
    
    JCheckBox cbox = new JCheckBox();
    JComboBox visType = new JComboBox();
    
    EquationAdmin eqAdm;
    
    OptionButtonPane optionButtonPane;
    
    // zum rechnen
//    pcalc polyCalc = new pcalc();
    
    // Ende Variablen
    
    public String getImpsurfCode(String strPoly, double[] scale) {
	String strRichMor = "DEF=" + URLEncoder.encode(strPoly + "=0;")
	    + "&XMIN=-" + (9.0 / scale[0]) + "&XMAX=" + (9.3 / scale[0])
	    + "&YMIN=-" + (9.1 / scale[1]) + "&YMAX=" + (9.1 / scale[1])
	    + "&ZMIN=-" + (9.2 / scale[2]) + "&ZMAX=" + (9.3 / scale[2])
	    + "" + "&COARSE=8&FINE=64&FACE=512&EDGE=4096"
	    + "&TIMEOUT=1000&VERSION=3";
	return (strRichMor);
    }
    
    public boolean triangulate(String strPoly, double[] scale,
			       String eindeutigeEqnNr) {
	// ??? do this on the webserver!!!
	//  System.out.println("scale:"+scale[0]+","+scale[1]+","+scale[2]);
	String strRichMor = getImpsurfCode(strPoly, scale);
	
	if (surfex_.configFrame.webPro.isSelected()) {
	    try {
		URL url = new URL(surfex_.webPrgs.surfPath.getText()
				  + "surfex_comp.php");
		URLConnection connection = url.openConnection();
		connection.setUseCaches(false);
		connection.setDoOutput(true);
		
		PrintWriter out = new PrintWriter(connection.getOutputStream());
		//        System.out.println("old subdir:"+surfex_.getCurrentProject().old_tmpsubdir);
		out
		    .print("prefix=surfex_tmp"
			   + "&filelocation="
			   + URLEncoder.encode("./" + eindeutigeEqnNr
					       + ".jvx")
			   + "&tmpsubdir="
			   + URLEncoder
			   .encode(surfex_.getCurrentProject().old_tmpsubdir)
			   + "&prg=impsurf" + "&prg_code_part1="
			   + URLEncoder.encode(strRichMor)
			   + "&prg_code_part2=");
		out.close();
		// read the file names
		BufferedReader in = new BufferedReader(new InputStreamReader(
							   connection.getInputStream()));
		String inputLine;
		inputLine = in.readLine();
		//        System.out.println(inputLine);
		String strFilename = "";
		if (inputLine.equals("okay")) {
		    //        System.out.println("it works!");
		    strFilename = in.readLine();
		    surfex_.getCurrentProject().old_tmpsubdir = in.readLine();
		    
		    System.out.println("achtung seit Version 020 geaendert damit er kompiliert - ohne test");
		    
		    //   eqAdm.img_filename = surfex_.webPrgs.surfPath.getText()               + strFilename;
		} else {
		    System.out.println("it does not work!");
		    System.out.println(in.readLine());
		}
		in.close();
	    } catch (Exception ex) {
		System.out.println("triang:" + ex.toString());
	    }
	} else {
	    try {
		File outputFile = new File("./impsurf.richmor");
		FileWriter out = new FileWriter(outputFile);
		int len = strRichMor.length();
		for (int pos = 0; pos < len; pos++) {
		    out.write(strRichMor.charAt(pos));
		}
		out.close();
		
		outputFile = new File("./impsurf.sh");
		out = new FileWriter(outputFile);
		String strCommand = "#!/bin/sh\n"
		    + "export REQUEST_METHOD=POST\n "
		    + "export CONTENT_LENGTH=" + (strRichMor.length())
		    + "\n " + "./localimpsurfCV <./impsurf.richmor >./"
		    + eindeutigeEqnNr + ".jvx";
		
		len = strCommand.length();
		for (int pos = 0; pos < len; pos++) {
		    out.write(strCommand.charAt(pos));
		}
		out.close();
	    } catch (Exception e2) {
		return (false);
	    }
	    
	    try {
		Runtime r = Runtime.getRuntime();
		Process p;
		p = r.exec("sh ./impsurf.sh");
		p.waitFor();
	    } catch (Exception e1) {
		System.out.println(e1.toString());
		return (false);
	    }
	}
	return (true);
    } // end of triangulate()
    
    // Konstruktor
    Equation(int internalEqnr, int eqnr, EquationAdmin ea, jv4surfex jv4sx,
	     surfex sx) {
	
//	polyCalc.doPrint = false;
	eqAdm = ea;
	this.eqnr = eqnr;
	this.jv4sx = jv4sx;
	surfex_ = sx;
	eindeutigeEqnNr = eqAdm.ProjectNumber + "_"
	    + (new Integer(eqnr)).toString();

	cbox.setSelected(true);

	visType.insertItemAt("normal",0);
	visType.insertItemAt("100% transparent",1);
	visType.insertItemAt("as clipping surface",2);
	visType.setSelectedIndex(0);

	final OptionButtonPane tempOptionButtonPane = new OptionButtonPane(
	    true, 10, 0);
	optionButtonPane = tempOptionButtonPane;
	basePlaneButton.setBackground(Color.green);
	basePlaneButton.setActionCommand("basePlaneButton" + internalEqnr);
	basePlaneButton.setToolTipText("Triangulate this surface");
	deletePlaneButton.setActionCommand("deletePlaneButton" + internalEqnr);
	deletePlaneButton.setToolTipText("Delete this surface");
	deletePlaneButton.setEnabled(false);
	optionButton.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent evt) {
		    tempOptionButtonPane.setVisible(true);
		}
	    });
	optionButton.setToolTipText("More options for the appearence");
	//  label1=new JLabel("Equation");
	label1 = new JLabel("f" + eqnr);
	text = new JTextField("");
	setLayout(new BorderLayout());
	//  this.add(labelpanel, BorderLayout.EAST);
///	labelpanel.add(label1);
	//  labelpanel.add(label2);
	this.add(text, BorderLayout.CENTER);
	this.add(configPanel, BorderLayout.WEST);
	//  if(!surfex_.inAnApplet) {
	//      configPanel.add(basePlaneButton);
	//  }
	configPanel.add(cbox);

	configPanel.add(visType);

	colorButton_i = new JButton();
	colorButton_i.setBackground(new Color(0, 180, 240));
	colorButton_i.setToolTipText("Select the \"inside\" color");
	colorButton_i.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent evt) {
		    colorButton_i.setBackground(JColorChooser.showDialog(null,
									 "Change surface's inside color", colorButton_i
									 .getBackground()));
		}
	    });
	
	colorButton_o = new JButton();
	colorButton_o.setBackground(new Color(240, 180, 0));
	colorButton_o.setToolTipText("Select the \"outside\" color");
	colorButton_o.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent evt) {
		    colorButton_o.setBackground(JColorChooser.showDialog(null,
									 "Change surface's outside color", colorButton_o
									 .getBackground()));
		}
	    });
	
	configPanel.add(colorButton_o);
	configPanel.add(colorButton_i);
//	configPanel.add(deletePlaneButton);
	configPanel.add(optionButton);
	configPanel.add(label1);
    }
    
    //Methoden
    public boolean isSelected() {
	return (cbox.isSelected());
    }
    
    public int getEqNo() {
	return (eqnr);
    }
    
    public void setEquationNo(int no) {
	eqnr = no;
	eindeutigeEqnNr = eqAdm.ProjectNumber + "_"
	    + (new Integer(eqnr)).toString();
	label1.setText("f" + eqnr);
    }
    
    public void updateActionCommands(int internalEqnr) {
	basePlaneButton.setActionCommand("basePlaneButton" + internalEqnr);
	deletePlaneButton.setActionCommand("deletePlaneButton" + internalEqnr);
    }
    
    public void setBasePlane(boolean b) {
	// erhaelt nur false ...
	basePlaneButton.setBackground(Color.white);
    }
    
    public void setBasePlaneDefault() {
	basePlaneButton.setBackground(Color.green);
	//  System.out.println("jv4sx.show()");
	jv4sx.showDefault();
	//  jv4sx.show();
    }
    
    public void setBasePlane(boolean b, jv4surfex jv4sx, double[] scale) {
	//  System.out.println("setBasePlane...");
	// erhaelt nur false ...
	triangulate(text.getText(), scale, eindeutigeEqnNr);
	basePlaneButton.setBackground(Color.green);
	String strtmpdir = ".";
	if (surfex_.inAnApplet) {
	    strtmpdir = surfex_.getCurrentProject().old_tmpsubdir;
	}
	if (jv4sx.loadAndDisplay(strtmpdir + "/" + eindeutigeEqnNr + ".jvx")) {
	    //          System.out.println("jv show");
	    jv4sx.show();
	} else {
	    jv4sx.showDefault();
	    //      System.out.println("jv problem");
	    //      jv4sx.loadAndDisplay("./impsurf.jvx");
	    jv4sx.show();
	}
    }
    
    public boolean isBasePlane() {
	//  System.out.println("bg:"+basePlaneButton.getBackground());
	return false;
	//  if (basePlaneButton.getBackground().equals(Color.green)) {
	//      return true;
	//  } else {
	//      return false;
	//  }
    }
    
    public String getPolyCode(String polyname) {
	return ("poly " + polyname + " = "
		+ text.getText().replaceAll("-", "-1*") + ";");
    }
    
    public String getSurfCode(String polyname, int surface_no, double ang[]) {
	String str = "";
    //  str += getPolyCode(polyname);
	String strSurfNo = "";
	if (surface_no > 1) {
	    strSurfNo = "" + surface_no;
	}
	str += "surface" + strSurfNo + " = " + polyname + ";\n";
	str += "surface" + strSurfNo + "_red = "
	    + (colorButton_o.getBackground().getRed()) + ";\n" + "surface"
	    + strSurfNo + "_green = "
	    + (colorButton_o.getBackground().getGreen()) + ";\n" + "surface"
	    + strSurfNo + "_blue = "
	    + (colorButton_o.getBackground().getBlue()) + ";\n";
	str += "inside" + strSurfNo + "_red = "
	    + (colorButton_i.getBackground().getRed()) + ";\n" + "inside"
	    + strSurfNo + "_green = "
	    + (colorButton_i.getBackground().getGreen()) + ";\n" + "inside"
	    + strSurfNo + "_blue = "
	    + (colorButton_i.getBackground().getBlue()) + ";\n";
	if(visType.getSelectedIndex()==1) {
	    str += "transparence" + strSurfNo + " = 100;\n";
	} else {
	    str += "transparence" + strSurfNo + " = "
		+ (new Integer(optionButtonPane.slider1.getValue())) + ";\n";
	}
	
	//  str += "surface" + strSurfNo + "=rotate(surface" + strSurfNo + ","
	//      + ang[0] + ",yAxis);";
	//  str += "surface" + strSurfNo + "=rotate(surface" + strSurfNo + ","
	//      + ang[1] + ",xAxis);";
	//  str += "surface" + strSurfNo + "=rotate(surface" + strSurfNo + ","
	//      + ang[2] + ",zAxis);";
	return (str);
    }
    
    public void savesurfCode(PrintWriter pw, String polyname, int surface_no,
			     double ang[]) {
	//  System.out.println("poly print");
	pw.println("poly " + polyname + " = "
		   + text.getText().replaceAll("-", "-1*") + ";");
	String strSurfNo = "";
	if (surface_no > 1) {
	    strSurfNo = "" + surface_no;
	}
	pw.println("surface" + strSurfNo + " = " + polyname + ";\n");
	pw.println("surface" + strSurfNo + "_red = "
		   + (colorButton_o.getBackground().getRed()) + ";\n" + "surface"
		   + strSurfNo + "_green = "
		   + (colorButton_o.getBackground().getGreen()) + ";\n" + "surface"
		   + strSurfNo + "_blue = "
		   + (colorButton_o.getBackground().getBlue()) + ";");
	pw.println("inside" + strSurfNo + "_red = "
		   + (colorButton_i.getBackground().getRed()) + ";\n" + "inside"
		   + strSurfNo + "_green = "
		   + (colorButton_i.getBackground().getGreen()) + ";\n" + "inside"
		   + strSurfNo + "_blue = "
		   + (colorButton_i.getBackground().getBlue()) + ";");
	pw.println("transparence" + strSurfNo + " = 0;");
	pw.println("surface" + strSurfNo + "=rotate(surface" + strSurfNo + ","
		   + ang[0] + ",yAxis);");
	pw.println("surface" + strSurfNo + "=rotate(surface" + strSurfNo + ","
		   + ang[1] + ",xAxis);");
	pw.println("surface" + strSurfNo + "=rotate(surface" + strSurfNo + ","
		   + ang[2] + ",zAxis);");
    }

    /*
    public void savePOVCode(PrintWriter pw) {
	//  System.out.println("poly print");
	String POVCode = "";
	pw.println("poly");
	pw.println("{");
	pw.println(polyCalc.showAsPovrayCode(text.getText()));
	//  pw.println(text.getText().substring(0,2)+",");
	//  pw.println("<"+text.getText().substring(3,text.getText().length())+">");
	pw.println(" pigment { rgb <"
		   + (colorButton_o.getBackground().getRed() / 255.0) + ","
		   + (colorButton_o.getBackground().getGreen() / 255.0) + ","
		   + (colorButton_o.getBackground().getBlue() / 255.0) + "> }");
	
	pw.println("finish { specular .6 }");
	pw.println("clipped_by {");
	pw.println("      sphere { < 0, 0, 0 >, Radius }");
	pw.println("}");
	pw.println("scale scale_x");
	pw.println("scale scale_y");
	pw.println("scale scale_z");
	pw.println("rotate y*degy");
	pw.println("rotate x*degx");
	pw.println("rotate z*degz");
	pw.println("}");
	}
    */
    
    public void saveYourself(PrintWriter pw) {
	pw.println("////////////////// Equation: /////////////////////////");
	pw.println("" + eqnr + "\n");
	pw.println(text.getText() + "\n");
	pw.println(colorButton_i.getBackground().getRed() + "\n");
	pw.println(colorButton_i.getBackground().getGreen() + "\n");
	pw.println(colorButton_i.getBackground().getBlue() + "\n");
	pw.println(colorButton_o.getBackground().getRed() + "\n");
	pw.println(colorButton_o.getBackground().getGreen() + "\n");
	pw.println(colorButton_o.getBackground().getBlue() + "\n");
	pw.println(basePlaneButton.getBackground().getRed() + "\n");
	pw.println(cbox.isSelected() + "\n");
	pw.println(visType.getSelectedIndex() + "\n");
	optionButtonPane.saveYourself(pw);
    }
    
    public String saveYourself() {
	String str = "";
	str += "////////////////// Equation: /////////////////////////" + "\n";
	str += ("" + eqnr + "\n");
	str += text.getText() + "\n";
	str += colorButton_i.getBackground().getRed() + "\n";
	str += colorButton_i.getBackground().getGreen() + "\n";
	str += colorButton_i.getBackground().getBlue() + "\n";
	str += colorButton_o.getBackground().getRed() + "\n";
	str += colorButton_o.getBackground().getGreen() + "\n";
	str += colorButton_o.getBackground().getBlue() + "\n";
	str += basePlaneButton.getBackground().getRed() + "\n";
	str += cbox.isSelected() + "\n";
	str += visType.getSelectedIndex() + "\n";
	str += optionButtonPane.saveYourself();
	return (str);
    }
    
    public void updateJV4SX() {
	//  System.out.println("updateJV4SX!");
	String strtmpdir = ".";
	if (surfex_.inAnApplet) {
	    strtmpdir = surfex_.getCurrentProject().old_tmpsubdir;
	}
	if (jv4sx.loadAndDisplay(strtmpdir + "/" + eindeutigeEqnNr + ".jvx")) {
	    //          System.out.println("jv show");
	    jv4sx.show();
	} else {
	    //      System.out.println("jv problem");
	    //      jv4sx.loadAndDisplay("./impsurf.jvx");
	    jv4sx.showDefault();
	    jv4sx.show();
	}
    }
} // end class Equation

