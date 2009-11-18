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
import java.awt.Container;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.net.MalformedURLException;
import java.net.URL;

import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;

//////////////////////////////////////////////////////////////
//
// class AboutFrame
//
//////////////////////////////////////////////////////////////

public class AboutFrame extends JFrame {
	surfex surfex_;

	AboutFrame(surfex sx) {
		super("about surfex");
		surfex_ = sx;
		//  setSize(550, 210);
		Container cp = getContentPane();
		cp.setLayout(new BorderLayout());
		JPanel panel1 = new JPanel();
		panel1.setLayout(new BorderLayout());
		try {
			if ((surfex_.filePrefix).equals("")) {
				panel1.add(new JLabel(new ImageIcon("Oliver_Labs_color.jpg")),
						BorderLayout.CENTER);
			} else {
				panel1.add(new JLabel(new ImageIcon(new URL(surfex_.filePrefix
						+ "Oliver_Labs_color.jpg"))), BorderLayout.CENTER);
			}
		} catch (MalformedURLException e) {
			System.out.println(e.toString());
		}
		//  panel1.add(new JLabel("Oliver Labs"), BorderLayout.SOUTH);
		panel1.add(new JLabel("     "));

		JPanel panel2 = new JPanel();
		panel2.setLayout(new BorderLayout());
		try {
			if ((surfex_.filePrefix).equals("")) {
				panel2.add(new JLabel(new ImageIcon("Oliver_Labs_color.jpg")),
						BorderLayout.CENTER);
			} else {
				panel2.add(new JLabel(new ImageIcon(new URL(surfex_.filePrefix
						+ "Oliver_Labs_color.jpg"))), BorderLayout.CENTER);
			}
		} catch (MalformedURLException e) {
			System.out.println(e.toString());
		}
		//  panel2.add(new JLabel("Stephan Holzer"), BorderLayout.SOUTH);
		panel2.add(new JLabel("     "));

		JPanel panel3 = new JPanel();
		panel3.setLayout(new GridLayout(13, 1));
		panel3.add(new JLabel(""));
		panel3.add(new JLabel("Surfex v" + surfex_.strVersion + " "
				+ surfex_.strDate));
		panel3.add(new JLabel(""));
		panel3.add(new JLabel("http://www.surfex.AlgebraicSurface.net"));
		panel3.add(new JLabel(""));
		panel3.add(new JLabel(
				"Programming and Design by: Stephan Holzer and Oliver Labs"));
		panel3.add(new JLabel(""));
		panel3.add(new JLabel("Johannes Gutenberg Universitaet (Mainz, Germany)"));
		panel3.add(new JLabel("and Universitaet des Saarlandes (Saarbruecken, Germany)"));
		panel3.add(new JLabel(""));
		panel3.add(new JLabel("Based on Code by Oliver Labs and Richard Morris"));
		panel3.add(new JLabel(""));

		JButton OKButton = new JButton("  OK  ");
		OKButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				setVisible(false);
			}
		});

		cp.add(panel3, BorderLayout.CENTER);
		cp.add(panel1, BorderLayout.WEST);
		cp.add(panel2, BorderLayout.EAST);
		cp.add(OKButton, BorderLayout.SOUTH);
		//  setLocation(70, 200);
		pack();
		setVisible(true);
		repaint();
	}
} // end class AboutFrame

