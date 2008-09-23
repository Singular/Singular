////////////////////////////////////////////////////////////////////////
//
// This file SolitaryPoint.java is part of SURFEX.
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
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.PrintWriter;

import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.JTextField;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

//////////////////////////////////////////////////////////////
//
// class SolitaryPoint
//
//////////////////////////////////////////////////////////////

public class SolitaryPoint extends JPanel {

	// Anfang Variablen

	Project project;

	public int solPtNo = 0;

	public JCheckBox cbox = new JCheckBox("", false);

	// GUI

	public JLabel nameLabel = null;

	double oldX = 0.0;

	double oldY = 0.0;

	double oldZ = 0.0;

	double newX = 0.0;

	double newY = 0.0;

	double newZ = 0.0;

	public JTextField x = new JTextField("0.00000");

	public JTextField y = new JTextField("0.00000");

	public JTextField z = new JTextField("0.00000");

	JLabel xLabel = new JLabel("x:");

	JLabel yLabel = new JLabel("y:");

	JLabel zLabel = new JLabel("z:");

	double oldFrom = 0.0;

	double oldTo = 5.0;

	double newFrom = oldFrom;

	double newTo = oldTo;

	public double radius = 0.25;

	public double oldRadius = radius;

	public JSlider radiusSlider = null;

	public JLabel radiusLabel = new JLabel("" + radius);

	public JComboBox surfaces;

	// Ende Variablen

	// Konstruktor
	SolitaryPoint(Project pro) {
		project = pro;

		setLayout(new BorderLayout());
		JPanel flowPanel = new JPanel(new FlowLayout());

		surfaces = new JComboBox(project.eqAdm.getEqNrs());

		//  flowPanel.add(cbox);

		nameLabel = new JLabel("s" + solPtNo);
		flowPanel.add(nameLabel);

		flowPanel.add(surfaces);

		radiusSlider = new JSlider(0, 1000);
		radiusSlider.setValue((int) Math.round(radius * 1000
				/ (newTo - newFrom)));
		radiusSlider.setMinorTickSpacing(1);
		radiusSlider.setMajorTickSpacing(10);
		radiusSlider.setPaintTicks(true);
		radiusSlider.addChangeListener(new ChangeListener() {
			public void stateChanged(ChangeEvent evt) {
				updateradiusSliderLabel();
			}
		});

		flowPanel.add(new JLabel("   "));

		x.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				try {
					double tmp = Double.parseDouble(x.getText());
					if (1 == 1) {
						tmp = Math.round(tmp * 1000) / 1000.0;
						oldX = tmp;
						newX = tmp;
						updateradiusSliderLabel();
					} else {
						x.setText("" + oldX);
					}
				} catch (Exception nanEx) {
					x.setText("" + oldX);
				}
			}
		});
		flowPanel.add(xLabel);
		flowPanel.add(x);

		y.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				try {
					double tmp = Double.parseDouble(y.getText());
					if (1 == 1) {
						tmp = Math.round(tmp * 1000) / 1000.0;
						oldY = tmp;
						newY = tmp;
						updateradiusSliderLabel();
					} else {
						y.setText("" + oldY);
					}
				} catch (Exception nanEx) {
					y.setText("" + oldY);
					//      System.out.println("oldTo:"+oldTo);
				}
			}
		});
		flowPanel.add(yLabel);
		flowPanel.add(y);

		z.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				try {
					double tmp = Double.parseDouble(z.getText());
					if (1 == 1) {
						tmp = Math.round(tmp * 1000) / 1000.0;
						oldZ = tmp;
						newZ = tmp;
						updateradiusSliderLabel();
					} else {
						z.setText("" + oldZ);
					}
				} catch (Exception nanEx) {
					z.setText("" + oldZ);
					//      System.out.println("oldTo:"+oldTo);
				}
			}
		});
		flowPanel.add(zLabel);
		flowPanel.add(z);

		flowPanel.add(radiusSlider);
		flowPanel.add(radiusLabel);

		add(flowPanel, BorderLayout.WEST);
	}

	public void updateradiusSliderLabel() {
		int end = 6;
		String str = ""
				+ (newFrom + radiusSlider.getValue() / 1000.0
						* (newTo - newFrom));
		if (str.length() <= 6) {
			end = str.length();
		}
		radiusLabel.setText(str.substring(0, end));
		project.solPtsAdm
				.copyRadiusToAll(newFrom, newTo, (newFrom + radiusSlider
						.getValue()
						/ 1000.0 * (newTo - newFrom)));
		this.repaint();
	}

	public boolean isSelected() {
		return (cbox.isSelected());
	}

	public void setSurfNo(int no) {
		surfaces.setSelectedIndex(no);
	}

	public String getSurfNo() {
		return (surfaces.getSelectedItem().toString());
	}

	public void setSolPtNo(int no) {
		solPtNo = no;
		nameLabel.setText("s" + solPtNo);
	}

	public void updateActionCommands(int internalCunr) {
	}

	public String getName() {
		return (nameLabel.getText());
	}

	public String getSurfCode() {
		String str = "poly " + nameLabel.getText() + " = " + "(x-("
				+ x.getText() + "))^2" + "+(y-(" + y.getText() + "))^2"
				+ "+(z-(" + z.getText() + "))^2" + "-(" + radiusLabel.getText()
				+ ")^2;\n";
		return (str);
	}

	public void saveYourself(PrintWriter pw) {
		String str = "";
		pw
				.println("////////////////// SolitaryPoint: /////////////////////////"
						+ "\n");

		pw.println("" + solPtNo + "\n");
		pw.println("" + getSurfNo() + "\n");
		pw.println("" + x.getText() + "\n");
		pw.println("" + y.getText() + "\n");
		pw.println("" + z.getText() + "\n");
		pw.println("" + newFrom + "\n");
		pw.println("" + newTo + "\n");
		pw.println("" + radiusSlider.getValue() + "\n");
		//  pw.println(radiusLabel.getText()+"\n");
		//  pw.println(cbox.isSelected()+"\n");
		//optionButtonPane.saveYourself(pw);
	}

	public String saveYourself() {
		String str = "";
		str += "////////////////// SolitaryPoint: /////////////////////////\n";
		str += ("" + solPtNo + "\n");
		str += ("" + getSurfNo() + "\n");
		str += ("" + x.getText() + "\n");
		str += ("" + y.getText() + "\n");
		str += ("" + z.getText() + "\n");
		str += ("" + newFrom + "\n");
		str += ("" + newTo + "\n");
		str += ("" + radiusSlider.getValue() + "\n");
		//  str += (radiusLabel.getText()+"\n");
		//  str += (cbox.isSelected()+"\n");
		return (str);
	}
} // end of class SolitaryPoint

