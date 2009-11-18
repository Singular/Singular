////////////////////////////////////////////////////////////////////////
//
// This file AboutFrame.java is part of SURFEX.
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
import java.io.PrintWriter;

import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JColorChooser;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JPanel;

//////////////////////////////////////////////////////////////
//
//class Curve
//
//////////////////////////////////////////////////////////////
public class Curve extends JPanel {
 
	// Anfang Variablen

	// GUI

	public JComboBox C[] = { new JComboBox(), new JComboBox(), new JComboBox(),
			new JComboBox(), new JComboBox(), new JComboBox(), new JComboBox(),
			new JComboBox(), new JComboBox(), new JComboBox() };//new
																// JComboBox[10];

	//JPanel configPanel = new JPanel();

	JButton optionButton = new JButton("opts");

	JButton deleteCurveButton = new JButton("del");

	JLabel label1, label2;

	String eindeutigeCuNr;

	JButton colorButton_i, colorButton_o;

	//JPanel labelpanel = new JPanel(new GridLayout(2, 1));

	int cunr;

	JCheckBox cbox = new JCheckBox();

	Project project;

	CurveAdmin cuAdm;

	OptionButtonPane optionButtonPane;

	// zum rechnen
//	pcalc polyCalc = new pcalc();

	// Ende Variablen
	// Konstruktor
	Curve(int internalCunr, int cunr, CurveAdmin ca, Project pro) {
		//System.out.println("testpl" );

//		polyCalc.doPrint = false;
		cuAdm = ca;
		this.project = pro;
		this.cunr = cunr;
		eindeutigeCuNr = cuAdm.ProjectNumber + "_"
				+ (new Integer(cunr)).toString();

		cbox.setSelected(true);
		final OptionButtonPane tempOptionButtonPane = new OptionButtonPane(
				true, 10, 0);
		optionButtonPane = tempOptionButtonPane;
		deleteCurveButton.setActionCommand("deleteCurveButton" + internalCunr);
		deleteCurveButton.setToolTipText("Delete this Curve");
		deleteCurveButton.setEnabled(false);
		//deleteCurveButton.addActionListener(new ActionListener() {
		//public void actionPerformed(ActionEvent evt) {
		//cunList.removeElementAt(internalEqnr - 1);
		//updateCurvePanel();
		//}
		//});

		optionButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				tempOptionButtonPane.setVisible(true);
			}
		});
		optionButton.setToolTipText("More options for the appearence");
		// ??? for the moment:
		// because curves need other options than surfaces!!!
		optionButton.setEnabled(false);
		//  label1=new JLabel("Equation");

		//  this.add(label1);

		JPanel configPanel = new JPanel();//new GridLayout(1,4));
		JPanel labelPanel = new JPanel(new GridLayout(1, 1));
		JPanel centerPanel = new JPanel(new GridLayout(1, 10));
		// Vector eqNrs=project.eqAdm.getEqNrs();
		int i;
		/*
		 * String cunrT; ListIterator
		 * li=project.eqAdm.getEqNrs().listIterator(); while(li.hasNext()){
		 * cunrT=((Integer)li.next()).toString(); for(i=0;i <=9;i++){
		 * C[i].addItem(cunrT);//=(new JComboBox(eqNrs)); centerPanel.add(C[i]); } }
		 */
		for (i = 0; i <= 9; i++) {
			C[i] = new JComboBox(project.eqAdm.getEqNrs());
			centerPanel.add(C[i]);
		}

		setLayout(new BorderLayout());
		//this.add(labelPanel, BorderLayout.WEST);
		this.add(centerPanel, BorderLayout.CENTER);
		this.add(configPanel, BorderLayout.WEST);

		label2 = new JLabel("C" + cunr);
		labelPanel.add(label2);
		//this.add(baseCurveButton);
		configPanel.add(cbox);
		colorButton_i = new JButton();
		colorButton_i.setBackground(new Color(0, 0, 0));
		colorButton_i.setToolTipText("Select the Curves color");
		colorButton_i.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				colorButton_i.setBackground(JColorChooser.showDialog(null,
						"Change curfe's color", colorButton_i.getBackground()));
			}
		});

		configPanel.add(colorButton_i);

		//this.add(colorButton_i);
		configPanel.add(deleteCurveButton);
		configPanel.add(optionButton);
		configPanel.add(label2);
	}

	//Methoden
	public boolean isSelected() {
		return (cbox.isSelected());
	}

	public void setCurveNo(int no) {
		cunr = no;
		eindeutigeCuNr = cuAdm.ProjectNumber + "_"
				+ (new Integer(cunr)).toString();
		label2.setText("C" + cunr);
	}

	public int getCurveNo() {
		return (cunr);
	}

	public void updateActionCommands(int internalCunr) {
		deleteCurveButton.setActionCommand("deleteCurveButton" + internalCunr);
	}

	public String getSurfCode(String polyname, int surface_no, double ang[]) {
		//System.out.println("surf_no:"+surface_no);

		String str = "";
		str += "curve_red = " + (colorButton_i.getBackground().getRed()) + ";"
				+ "curve_green = " + (colorButton_i.getBackground().getGreen())
				+ ";" + "curve_blue = "
				+ (colorButton_i.getBackground().getBlue()) + ";";
		//str += "surface" + strSurfNo + "=rotate(surface" + strSurfNo + ","
		//+ ang[0] + ",yAxis);";
		//str += "surface" + strSurfNo + "=rotate(surface" + strSurfNo + ","
		//+ ang[1] + ",xAxis);";
		//str += "surface" + strSurfNo + "=rotate(surface" + strSurfNo + ","
		//+ ang[2] + ",zAxis);";
		int i;
		//System.out.println((C[0].getSelectedItem()).toString());

		str += "surf_nr = " + surface_no + ";\n";
		str += "cut_distance = 0.0001;\n";
		str += "curve_width = 2.5;\n";

		for (i = 1; i <= 8; i++) {
			if (((C[i].getSelectedItem()).toString()).equals("-")) {
				str += "cutsurface" + (i) + " = 0;";
			} else {
				str += "cutsurface" + (i) + " = f"
						+ ((C[i].getSelectedItem()).toString()) + ";";
				str += "cutsurface" + (i) + "=rotate(cutsurface" + (i) + ","
						+ ang[0] + ",yAxis);\n";
				str += "cutsurface" + (i) + "=rotate(cutsurface" + (i) + ","
						+ ang[1] + ",xAxis);\n";
				str += "cutsurface" + (i) + "=rotate(cutsurface" + (i) + ","
						+ ang[2] + ",zAxis);\n";
			}
		}
		str += "cut_with_surface;\n";

		return (str);
	}

	public void savesurfCode(PrintWriter pw, String polyname, int surface_no,
			double ang[]) {
		//mach surfcode
	}

	public void savePOVCode(PrintWriter pw) {
		// mach pov-code
	}

	public void saveYourself(PrintWriter pw) {
		String str = "";
		pw.println("////////////////// Curve: /////////////////////////");
		int i;
		pw.println("" + eindeutigeCuNr + "\n");
		for (i = 0; i <= 9; i++) {
			pw.println(C[i].getSelectedIndex());
		}
		pw.println(colorButton_i.getBackground().getRed());
		pw.println(colorButton_i.getBackground().getGreen());
		pw.println(colorButton_i.getBackground().getBlue());
		pw.println(cbox.isSelected());
		//optionButtonPane.saveYourself(pw);
	}

	public String saveYourself() {
		String str = "";
		str += "////////////////// Curve: /////////////////////////\n";
		int i;
		str += ("" + cunr + "\n");
		for (i = 0; i <= 9; i++) {
			str += C[i].getSelectedIndex() + "\n";
		}
		str += colorButton_i.getBackground().getRed() + "\n";
		str += colorButton_i.getBackground().getGreen() + "\n";
		str += colorButton_i.getBackground().getBlue() + "\n";
		str += cbox.isSelected() + "\n";
		//optionButtonPane.saveYourself(pw);
		return (str);
	}
} // end class Curve

