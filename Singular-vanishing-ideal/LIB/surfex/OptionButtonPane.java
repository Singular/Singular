////////////////////////////////////////////////////////////////////////
//
// This file OptionButtonPane.java is part of SURFEX.
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
import java.awt.Container;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.PrintWriter;
import java.util.Hashtable;

import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.JSlider;
import javax.swing.JTextField;

//////////////////////////////////////////////////////////////
//
// class OptionButtonPane
//
//////////////////////////////////////////////////////////////

public class OptionButtonPane extends JFrame {

	ButtonGroup group = new ButtonGroup();

	JRadioButton bt1;

	JRadioButton bt2;

	JTextField clipingRadiusTextField;

	JSlider slider1;

	public void saveYourself(PrintWriter pw) {
		pw.println(bt1.isSelected());
		pw.println(clipingRadiusTextField.getText());
		pw.println(slider1.getValue());
	}

	public String saveYourself() {
		String str = "";
		str += bt1.isSelected() + "\n";
		str += clipingRadiusTextField.getText() + "\n";
		str += slider1.getValue() + "\n";
		return (str);
	}

	OptionButtonPane(boolean preselectedAppearance,
			double presettedClipingRadius, int presettedSliderValue) {
		super("More Options for the appearance");

		setSize(400, 170);
		Container cp = getContentPane();
		cp.setLayout(new BorderLayout());

		JPanel panel3 = new JPanel();
		panel3.setLayout(new GridLayout(5, 3));
		panel3.add(new JLabel("               Clip surface with"));

		if (preselectedAppearance) {
			bt1 = new JRadioButton("Ball", true);
			bt2 = new JRadioButton("Box");
		} else {
			bt1 = new JRadioButton("Ball");
			bt2 = new JRadioButton("Box", true);
		}
		bt1.setEnabled(false);
		bt2.setEnabled(false);
		group.add(bt1);
		panel3.add(bt1);
		panel3.add(new JLabel(""));
		group.add(bt2);
		panel3.add(bt2);
		panel3.add(new JLabel("               cliping radius"));
		clipingRadiusTextField = new JTextField((String) Double
				.toString(presettedClipingRadius));
		clipingRadiusTextField.setEnabled(false);
		panel3.add(clipingRadiusTextField);

		panel3.add(new JLabel("               transparence"));
		slider1 = new JSlider(0, 100);
		slider1.setValue(presettedSliderValue);
		slider1.setMinorTickSpacing(1);
		slider1.setMajorTickSpacing(10);
		slider1.setPaintTicks(true);
		// Bei 50 und 100 einen Text setzen
		Hashtable labelTable = new Hashtable();
		labelTable.put(new Integer(50), new JLabel("50%"));
		labelTable.put(new Integer(100), new JLabel("100%"));
		slider1.setLabelTable(labelTable);
		panel3.add(slider1);

		JButton OKButton = new JButton("OK");
		OKButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				setVisible(false);
			}
		});

		cp.add(panel3, BorderLayout.CENTER);
		cp.add(OKButton, BorderLayout.SOUTH);
		setLocation(70, 200);
		setVisible(false);
		repaint();
	}
} // end class OptionButtonPane

