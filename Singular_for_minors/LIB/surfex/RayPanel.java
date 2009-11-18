////////////////////////////////////////////////////////////////////////
//
// This file RayPanel.java is part of SURFEX.
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

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.Toolkit;
import java.net.URL;

import javax.swing.*;
import javax.swing.SwingUtilities;

//////////////////////////////////////////////////////////////
//
// class RayPanel
//
//////////////////////////////////////////////////////////////

public class RayPanel extends JPanel {
	private Image theIMG = null;

	Graphics offGraphics;

	Dimension offDimension;

	Image offImage;
	
	

	String picname = "";

	RayPanel() {
	} 
	
	
	
	public void changeBackground(String pn) {
		picname = pn;
		if (theIMG != null) {
			theIMG.flush();
			theIMG = null;
		}
		//     if (theIMG != null) {
		//       theIMG.flush();
		//     }
		//    theIMG = Toolkit.getDefaultToolkit().getImage(picname);

		SwingUtilities.updateComponentTreeUI(this);
		repaint();
	}

	public void paint(Graphics g) {
		update(g);
	}

	//
	public void update(Graphics g) {
		//  System.out.println("update...");
		if (1 == 1) {
			Dimension d = getSize();
			if ((offGraphics == null) || (d.width != offDimension.width)
					|| (d.height != offDimension.height)) {
				offDimension = d;
				offImage = createImage(d.width, d.height);
				offGraphics = offImage.getGraphics();
			}

			if (theIMG == null) {
				//  System.out.println("null IMG:"+picname+"...");
				if (picname.length() < 5) {
					if (picname.length() > 0) {
						//  System.out.println("get IMG"+picname+"...");
					    theIMG = Toolkit.getDefaultToolkit().getImage(picname);
					    //  System.out.println("got IMG:"+picname+".");
					}
				} else {
					if ((picname.substring(0, 5)).equals("http:")) {
						//  System.out.println("try to load:"+picname);
						try {
							theIMG = Toolkit.getDefaultToolkit().getImage(
									new URL(picname));
							//    System.out.println("got image");
						} catch (Exception ex) {
							System.out.println(ex.toString());
						}
					} else {
						//  System.out.println("get IMG"+picname+"...");
						theIMG = Toolkit.getDefaultToolkit().getImage(picname);
						//  System.out.println("got IMG:"+picname+".");
						//      theIMG = Toolkit.getDefaultToolkit().getImage(picname);
					}
				}
			}
			if (theIMG != null) {
				//  System.out.println("draw IMG");
				//    offGraphics.setColor(Color.black);
				//    offGraphics.clearRect(0,0,d.width,d.height);
				//    offGraphics.setClip(0,0,d.width,d.height);
				offGraphics.drawImage(theIMG, 0, 0, d.width, d.height, this);
				//    System.out.println("image drawn");
			}

			g.drawImage(offImage, 0, 0, this);
		}
	}
} // end class RayPanel

