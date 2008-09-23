////////////////////////////////////////////////////////////////////////
//
// This file SolitaryPointsAdmin.java is part of SURFEX.
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
// class SolitaryPointsAdmin
//
//////////////////////////////////////////////////////////////

public class SolitaryPointsAdmin extends JPanel implements ActionListener {

	JPanel solPtsPanel;

	public Vector solPtsList = new Vector();

	Project project;

	surfex surfex_;

	SolitaryPointsAdmin(surfex su, Project pro) {
		surfex_ = su;
		project = pro;

		setLayout(new BorderLayout());
		solPtsPanel = new JPanel(new GridLayout(20, 0));
		add(solPtsPanel, BorderLayout.WEST);
	}

	public void actionPerformed(ActionEvent e) {
	}

   public void clear() {
	solPtsList.clear();
	solPtsPanel.removeAll();
    }


	public void newSolitaryPoint() {
		//  System.out.println("newSolitaryPoint");
		SolitaryPoint par = new SolitaryPoint(project);

		// find a non-used parameter number:
		int n = solPtsList.size() + 1;
		ListIterator li = solPtsList.listIterator();
		while (li.hasNext()) {
			if (n == ((SolitaryPoint) (li.next())).solPtNo) {
				// if the parameter number n is already in use
				n = n + 1; // try the next number
				// reset the list iterator to the first element in the list:
				while (li.hasPrevious()) {
					li.previous();
				}
			}
		}
		par.setSolPtNo(n);
		//  System.out.println("solPtNo:"+n);

		// add this parameter to the list:
		solPtsList.add(par);
		//  System.out.println("solPtsList:"+solPtsList.size());
		solPtsPanel.add(par);
		SwingUtilities.updateComponentTreeUI(this);
	}

	public void equationListChanged_add(String EqName) {
		ListIterator li = solPtsList.listIterator();
		SolitaryPoint pt;
		int i;
		while (li.hasNext()) {
			//       System.out.println("update pla oe" );
			pt = (SolitaryPoint) li.next();
			pt.surfaces.addItem(EqName);
		}
	}

	public String save() {
		String str = "";
		ListIterator li = solPtsList.listIterator();
		str += "////////////////// SOLITARY POINTS: /////////////////////////"
				+ "\n";
		str += solPtsList.size() + "\n";
		while (li.hasNext()) {
			str += ((SolitaryPoint) li.next()).saveYourself();
		}
		return (str);
	}

	public void copyRadiusToAll(double from, double to, double rad) {
		int i;
		SolitaryPoint tmp;
		for (i = 0; i < solPtsList.size(); i++) {
			try {
				//    System.out.println("i:"+i);
				tmp = ((SolitaryPoint) (solPtsList.elementAt(i)));
				((SolitaryPoint) (solPtsList.elementAt(i))).newFrom = from;
				((SolitaryPoint) (solPtsList.elementAt(i))).oldFrom = from;
				((SolitaryPoint) (solPtsList.elementAt(i))).newTo = to;
				((SolitaryPoint) (solPtsList.elementAt(i))).oldTo = to;
				((SolitaryPoint) (solPtsList.elementAt(i))).radiusSlider
						.setValue((int) (rad / (to - from) * 1000.0));
				((SolitaryPoint) (solPtsList.elementAt(i))).radiusSlider
						.repaint();
				String str = "" + rad;
				int end = 6;
				if (str.length() <= 6) {
					end = str.length();
				}
				((SolitaryPoint) (solPtsList.elementAt(i))).radiusLabel
						.setText(str.substring(0, end));
			} catch (Exception ex) {
				System.out.println("error:" + ex.toString());
			}
		}
	}

	public void loadSolitaryPoints(BufferedReader bs, String datatype,
				int iVersion) {
		int k;
		try {
			String nix = bs.readLine();
			String zeile1;
			boolean first = true;

			for (k = Integer.parseInt(bs.readLine()); k > 0; k--) {
				zeile1 = bs.readLine();//) != null) {
				newSolitaryPoint();
				if (datatype.equals("specify")) {
					String strline;
					String tmpline;
					try {
						bs.mark(255);
						strline = bs.readLine();
						while ((strline.equals("solPtNo:"))
								|| (strline.equals("fromtoval:"))
								|| (strline.equals("surface:"))
								|| (strline.equals("coords:"))) {
							//          System.out.println("strline:"+strline);
							if (strline.equals("solPtNo:")) {
								((SolitaryPoint) solPtsList.lastElement())
										.setSolPtNo(Integer.parseInt(bs
												.readLine()));
							}
							if (strline.equals("surface:")) {
								//        System.out.println("set surface...");
								tmpline = bs.readLine();
								if (tmpline.equals("-")) {
									((SolitaryPoint) solPtsList.lastElement())
											.setSurfNo(0);
								} else {
									//            System.out.println("set surface:"+(Integer.parseInt(tmpline)));
									((SolitaryPoint) solPtsList.lastElement())
											.setSurfNo(Integer
													.parseInt(tmpline));
								}
							}
							if (strline.equals("coords:")) {
								((SolitaryPoint) solPtsList.lastElement()).x
										.setText(bs.readLine());
								((SolitaryPoint) solPtsList.lastElement()).newX = Double
										.parseDouble(((SolitaryPoint) solPtsList
												.lastElement()).x.getText());
								((SolitaryPoint) solPtsList.lastElement()).oldX = Double
										.parseDouble(((SolitaryPoint) solPtsList
												.lastElement()).x.getText());
								((SolitaryPoint) solPtsList.lastElement()).y
										.setText(bs.readLine());
								((SolitaryPoint) solPtsList.lastElement()).newY = Double
										.parseDouble(((SolitaryPoint) solPtsList
												.lastElement()).y.getText());
								((SolitaryPoint) solPtsList.lastElement()).oldY = Double
										.parseDouble(((SolitaryPoint) solPtsList
												.lastElement()).y.getText());
								((SolitaryPoint) solPtsList.lastElement()).z
										.setText(bs.readLine());
								((SolitaryPoint) solPtsList.lastElement()).newZ = Double
										.parseDouble(((SolitaryPoint) solPtsList
												.lastElement()).z.getText());
								((SolitaryPoint) solPtsList.lastElement()).oldZ = Double
										.parseDouble(((SolitaryPoint) solPtsList
												.lastElement()).z.getText());
							}
							if (strline.equals("fromtoval:")) {
								((SolitaryPoint) solPtsList.lastElement()).oldFrom = Double
										.parseDouble(bs.readLine());
								((SolitaryPoint) solPtsList.lastElement()).newFrom = ((SolitaryPoint) solPtsList
										.lastElement()).oldFrom;

								((SolitaryPoint) solPtsList.lastElement()).oldTo = Double
										.parseDouble(bs.readLine());
								((SolitaryPoint) solPtsList.lastElement()).newTo = ((SolitaryPoint) solPtsList
										.lastElement()).oldTo;

								((SolitaryPoint) solPtsList.lastElement()).radiusSlider
										.setValue(Integer.parseInt(bs
												.readLine()));
								((SolitaryPoint) solPtsList.lastElement())
										.updateradiusSliderLabel();
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
					((SolitaryPoint) solPtsList.lastElement())
							.setSolPtNo(Integer.parseInt(bs.readLine()));
					((SolitaryPoint) solPtsList.lastElement())
							.setSurfNo(Integer.parseInt(bs.readLine()));
					((SolitaryPoint) solPtsList.lastElement()).x.setText(bs
							.readLine());
					((SolitaryPoint) solPtsList.lastElement()).newX = Double
							.parseDouble(((SolitaryPoint) solPtsList
									.lastElement()).x.getText());
					((SolitaryPoint) solPtsList.lastElement()).oldX = Double
							.parseDouble(((SolitaryPoint) solPtsList
									.lastElement()).x.getText());
					((SolitaryPoint) solPtsList.lastElement()).y.setText(bs
							.readLine());
					((SolitaryPoint) solPtsList.lastElement()).newY = Double
							.parseDouble(((SolitaryPoint) solPtsList
									.lastElement()).y.getText());
					((SolitaryPoint) solPtsList.lastElement()).oldY = Double
							.parseDouble(((SolitaryPoint) solPtsList
									.lastElement()).y.getText());
					((SolitaryPoint) solPtsList.lastElement()).z.setText(bs
							.readLine());
					((SolitaryPoint) solPtsList.lastElement()).newZ = Double
							.parseDouble(((SolitaryPoint) solPtsList
									.lastElement()).z.getText());
					((SolitaryPoint) solPtsList.lastElement()).oldZ = Double
							.parseDouble(((SolitaryPoint) solPtsList
									.lastElement()).z.getText());
					((SolitaryPoint) solPtsList.lastElement()).oldFrom = Double
							.parseDouble(bs.readLine());
					((SolitaryPoint) solPtsList.lastElement()).newFrom = ((SolitaryPoint) solPtsList
							.lastElement()).oldFrom;
					((SolitaryPoint) solPtsList.lastElement()).oldTo = Double
							.parseDouble(bs.readLine());
					((SolitaryPoint) solPtsList.lastElement()).newTo = ((SolitaryPoint) solPtsList
							.lastElement()).oldTo;
					((SolitaryPoint) solPtsList.lastElement()).radiusSlider
							.setValue(Integer.parseInt(bs.readLine()));
					((SolitaryPoint) solPtsList.lastElement())
							.updateradiusSliderLabel();
					((SolitaryPoint) solPtsList.lastElement()).repaint();
				}
				if (datatype.equals("complete") || datatype.equals("onlystyle")) {
					//        ((SolitaryPoint) solPtsList.lastElement()).cbox.setSelected((Boolean.valueOf(bs.readLine())).booleanValue());
				}
			}
		} catch (IOException e) {
			System.out.println(e);
		}
	}

} // end class SolitaryPointsAdmin
