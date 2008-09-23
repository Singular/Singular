////////////////////////////////////////////////////////////////////////
//
// This file RayFrame.java is part of SURFEX.
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
import java.awt.Image;

import javax.swing.JFrame;
import javax.swing.JSlider;
import javax.swing.JTextField;

import javax.swing.*;
//////////////////////////////////////////////////////////////
//
// class RayFrame
//
//////////////////////////////////////////////////////////////

public class RayFrame extends JFrame {
  private Image theIMG = null;

  //UpdateRayframeImmediatlyThread updateRayframeImmediatlyThread;

  public RayPanel panel;

  public JLabel status=new JLabel();
  public JLabel info=new JLabel(" ");

  long tic,toc=0;
  int fps=0;

  int rand;
  
  RayFrame(String name) {
	
    super(name);
    //updateThread=uT;
    Container cp = getContentPane();
    cp.setLayout(new BorderLayout());
    panel = new RayPanel();
    cp.add(panel,BorderLayout.CENTER);
    isWorking(false);
    cp.add(status,BorderLayout.SOUTH);
    setLocation(550, 200);
    repaint();
    rand=(int)(Math.random()*1000000);
  }
  
  
	

  
  public void setInfo(String s){
  	info.setText(s);
  }
  
  public void isWorking(boolean b){
  	if(b){
  		tic=System.currentTimeMillis();
  		status.setText("updating   "+fps+"fps");
  	
  	}else{
  		status.setText(" ");
  		toc=System.currentTimeMillis();
//  		System.out.println();
  		if(toc-tic !=0){
  		fps=(int)(1000/(toc-tic)*1.2); // 1.2 wie lange die anderen sachen ca. wegfressen
  		}else{
  			fps=2147483647;
  		}
  		tic=toc;
  	}
  }

  public void changeBackground(String picname) {
      //    System.out.println("name:"+picname);
    //  System.out.println("cp:"+getContentPane().getSize());
    //  System.out.println("ins:"+getInsets());
    //  System.out.println("size:"+getSize());
    //  panel.setSize(getContentPane().getSize());
    panel.changeBackground(picname);
  }
} // end class RayFrame


