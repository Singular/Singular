////////////////////////////////////////////////////////////////////////
//
// This file CurveAdmin.java is part of SURFEX.
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
import java.io.IOException;
import java.util.ListIterator;
import java.util.Vector;

import javax.swing.JPanel;
import javax.swing.SwingUtilities;

//////////////////////////////////////////////////////////////
//
// class CurveAdmin
//
//////////////////////////////////////////////////////////////

public class CurveAdmin extends JPanel implements ActionListener {
	//   Anfang Variablen
	// GUI
	Curve oldcu;

	private surfex surfex_;

	// this Project
	Vector cunList = new Vector();

	int cuncounter = 0;

	Project project;

	JPanel cuPanel = new JPanel();

	int ProjectNumber;

	String projectName;

	String filename;

	//Ende Variablen

	// Konstruktoren
	CurveAdmin(surfex su) {
		surfex_ = su;
		projectName = "Project 1";

	}

	public void equationListChanged_add(String EqName) {
		ListIterator li = cunList.listIterator();
		Curve cu;
		int i;
		while (li.hasNext()) {
			//       System.out.println("update pla oe" );
			cu = (Curve) li.next();
			for (i = 0; i <= 9; i++) {
				//             System.out.println(i);
				cu.C[i].addItem(EqName);
			}
		}
	}

	public void actionPerformed(ActionEvent e) {
		// chekce ob Curve deleted wurde
	}

    public void clear() {
	cunList.clear();
	cuncounter = 0;
	cuPanel.removeAll();
    }
 


	public Vector getEqNrs() {
		Vector temp = new Vector();
		ListIterator li = cunList.listIterator();
		while (li.hasNext()) {
			temp.add(new Integer(((Curve) li.next()).cunr));
		}
		return temp;
	}

	CurveAdmin(int i, String filename, surfex su, Project pro) {
		project = pro;
		this.filename = filename;
		ProjectNumber = i;
		projectName = "Project" + i;
		this.surfex_ = su;

		this.setLayout(new BorderLayout());

		cuPanel.setLayout(new GridLayout(20, 0));
		//  newCurve();
		//  oldcu = (Curve) cunList.firstElement();
		oldcu = null;

		this.add(cuPanel);

	}

    /*
	public void savePovCode(String filename) {
		/*  // erzeuge POVCode und
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
		 ListIterator li = cunList.listIterator();
		 Curve thisEqn;
		 while (li.hasNext()) {
		 thisEqn = (Curve) li.next();
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

	public void saveSurfCode(String filename, String imgFilename) {
		/*try {

		 FileOutputStream fo = new FileOutputStream(filename);

		 PrintWriter pw = new PrintWriter(fo, true);

		 pw.println("scale_x = " + scale[0] + ";");
		 pw.println("scale_y = " + scale[1] + ";");
		 pw.println("scale_z = " + scale[2] + ";");
		 pw.println("radius=9.0;");

		 pw
		 .println("illumination = ambient_light + diffuse_light + reflected_light + transmitted_light;");
		 pw
		 .println("ambient      = 40;\ndiffuse      = 80;\nreflected    = 80;\ntransmitted  = 20;");
		 pw.println("smoothness   = 50;\ntransparence = 0;");
		 pw.println("thickness = 10;");
		 pw
		 .println("background_red=0; background_green=0; background_blue=0;");
		 // Polynome aus der Liste uebergebn
		 ListIterator li = cunList.listIterator();
		 Curve thisEqn;
		 int lc = 0;
		 int ec = 1;
		 while (li.hasNext()) {
		 thisEqn = (Curve) li.next();
		 if (thisEqn.isSelected()) {
		 if (lc <= 9) {
		 thisEqn.savesurfCode(pw, "f" + ec, lc + 1, jv4sx
		 .getCameraRotationYXZ());
		 lc++;
		 }
		 }
		 ec++;
		 }

		 pw.println("draw_surface;");
		 pw.println("filename=\""+imgFilename+"\";");
		 //  pw.println("color_file_format=jpg;");
		 pw.println("save_color_image;");

		 pw.close();
		 } catch (IOException er) {
		 System.out.println(er);
		 } */
	}

	public void updateCurvePanel() {
		cuPanel.removeAll();
		cuPanel.setLayout(new GridLayout(20, 0));
		//  System.out.println("updatepanel");
		ListIterator li = cunList.listIterator();
		Curve cu;
		int internalCunr = 1;
		while (li.hasNext()) {
			cu = (Curve) li.next();
			cu.updateActionCommands(internalCunr++);
			cuPanel.add(cu);
		}
		SwingUtilities.updateComponentTreeUI(this);
	}

	public String vec2Str(double[] v) {
		return ("<" + v[0] + "," + v[1] + "," + v[2] + ">");
	}

	public void newCurve() {
		//int internalCunr, int cunr, CurveAdmin ca, Project pro
		Curve cun = new Curve(cunList.size() + 1, ++cuncounter, this, project);
		cun.colorButton_i.setBackground(project.appearanceScheme
				.getCurveColor(cun.getCurveNo()));

		//  System.out.println("new curve" );
		cuPanel.add(cun);
		cunList.add(cun);
		//    tbuttons.add(cun.deletePlaneButton);
		cun.deleteCurveButton.addActionListener(this);
		SwingUtilities.updateComponentTreeUI(this);
	}

	public void loadCurves(BufferedReader bs, String datatype,
				int iVersion) {
		int j, k;

		try {
			String nix = bs.readLine();
			String zeile1;
			//      boolean first = true;
			int numcu = Integer.parseInt(bs.readLine());
			for (k = numcu; k > 0; k--) {
				zeile1 = bs.readLine();//) != null) {
				//    if (first) {
				//        // lasse erstes element ueberschreiben
				//        first = false;
				//    } else {
				newCurve();
				//    }
				if (datatype.equals("specify")) {
					String strline;
					try {
						bs.mark(255);
						strline = bs.readLine();
						while ((strline.equals("curveno:"))
								|| (strline.equals("surfaces:"))
								|| (strline.equals("color:"))
								|| (strline.equals("showcbox:"))) {
							//          System.out.println("strline:"+strline);
							if (strline.equals("curveno:")) {
								((Curve) cunList.lastElement())
										.setCurveNo(Integer.parseInt(bs
												.readLine()));
							}
							if (strline.equals("surfaces:")) {
								for (j = 0; j <= 9; j++) {
									try {
										bs.mark(255);
										((Curve) cunList.lastElement()).C[j]
												.setSelectedIndex(Integer
														.parseInt(bs.readLine()));
									} catch (Exception nex) {
										bs.reset();
									}
								}
							}
							if (strline.equals("color:")) {
								((Curve) cunList.lastElement()).colorButton_i
										.setBackground(new Color(
												Integer.parseInt(bs.readLine()),
												Integer.parseInt(bs.readLine()),
												Integer.parseInt(bs.readLine())));
							}
							if (strline.equals("showcbox:")) {
								((Curve) cunList.lastElement()).cbox
										.setSelected((Boolean.valueOf(bs
												.readLine())).booleanValue());
							}
							bs.mark(255);
							strline = bs.readLine();
						} // end while()
						bs.reset();
					} catch (Exception nex) {
						bs.reset();
					}
				}
				if (datatype.equals("complete") || datatype.equals("onlyeqns")
						|| datatype.equals("eqnsvis")) {
					((Curve) cunList.lastElement()).setCurveNo(Integer
							.parseInt(bs.readLine()));
					for (j = 0; j <= 9; j++) {
						try {
							bs.mark(255);
							((Curve) cunList.lastElement()).C[j]
									.setSelectedIndex(Integer.parseInt(bs
											.readLine()));
						} catch (Exception nex) {
							bs.reset();
						}
						//        System.out.println(((Curve) cunList.lastElement()).C[j].getSelectedIndex() );
					}
				}
				if (datatype.equals("complete") || datatype.equals("onlystyle")) {
					((Curve) cunList.lastElement()).colorButton_i
							.setBackground(new Color(Integer.parseInt(bs
									.readLine()), Integer.parseInt(bs
									.readLine()), Integer.parseInt(bs
									.readLine())));
				}
				if (datatype.equals("complete") || datatype.equals("onlystyle")
						|| datatype.equals("eqnsvis")) {
					((Curve) cunList.lastElement()).cbox.setSelected((Boolean
							.valueOf(bs.readLine())).booleanValue());
				}
			}
		} catch (IOException e) {
			System.out.println("loadCurves:" + e);
		}
	}

	public String save() {
		String str = "";
		//  System.out.println("save cu" );
		//pw.println("String");
		ListIterator li = cunList.listIterator();
		str += "////////////////// CURVES: /////////////////////////" + "\n";
		str += cunList.size() + "\n";
		while (li.hasNext()) {
			str += ((Curve) li.next()).saveYourself();
		}
		return (str);
	}

} // end class CurveAdmin

