////////////////////////////////////////////////////////////////////////
//
// This file SavePicDialog.java is part of SURFEX.
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
import java.awt.Container;
import java.awt.FlowLayout;
import java.awt.GridLayout;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.io.File;

import javax.swing.AbstractButton;
import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.*;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.JTextField;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.filechooser.FileFilter;

//////////////////////////////////////////////////////////////
//
// class SavePicDialog
//
//////////////////////////////////////////////////////////////

public class SavePicDialog extends JFrame {
    RayFrame rayFrame;
    
    JTextField tf;
    
    int heightvalue=0;
    int widthvalue=0;
    
    JComboBox outputFormat;
    
    JComboBox colorType;
    
    JComboBox antialiasingQuality;
    
    JComboBox standardDim;
    
    JComboBox measure;
    int oldMeasure;
    
    JPanel panel = new JPanel();/*
				  
//	JRadioButton povCode = new JRadioButton("PovRay code");

//JRadioButton surfCode = new JRadioButton("Surf code");

//JRadioButton raytracedPic = new JRadioButton("Raytraced pic", true);

//JRadioButton rb[] = new JRadioButton[11];

//int aufloesungen[][] = { { 300, 300 }, { 600, 600 }, { 900, 900 },
//		{ 1200, 1200 }, { 240, 240 }, { 360, 360 }, { 480, 480 },
//		{ 640, 640 } };

				*/	
    ChangeListener CL;
    
    //boolean last[] = { true, false, false, false, false, false, false, false,
    //		false, false, false, true, false, false, true, false };
    
    //boolean actionWasPerformed = false;
    
    JTextField width;
    
    JTextField height;
    
    JCheckBox aspectRatio = new JCheckBox("Preserve aspect ratio", false);
    
    //JRadioButton unitPixels = new JRadioButton("Pixels", true);
    
    JTextField dpi = new JTextField("300");
    
    //JRadioButton unitCm = new JRadioButton("cm");
    
    //JRadioButton unitInches = new JRadioButton("inches");
    
    //int i, j;
    
    //JCheckBox ditheredCb = new JCheckBox("dithered - black/white", false);
    
    //public JCheckBox antialiasing = new JCheckBox("antialiasing", false);
    
    JButton SaveButton = new JButton("create and save as");
    
    JButton CancelButton = new JButton("cancel");
    
    JButton DefaultButton = new JButton("default");
    
    JLabel newSize = new JLabel();
    
    Project project; 
    
    surfex surfex_;
    
    String projectName;
    
    SavePicDialog(String proName, RayFrame ray, Project pro, surfex su) {
	super("save picture" + proName);
	surfex_ = su;
	rayFrame = ray;
	project = pro;
	this.projectName = proName;
	//setSize(790, 400);
	
	JPanel contentPane = (JPanel) getContentPane();
	JPanel contentPane1l = new JPanel(new BorderLayout());
	JPanel contentPane1r = new JPanel(new BorderLayout());
	
	JPanel contentPane2 = new JPanel(new BorderLayout());
	JPanel contentPane3 = new JPanel(new BorderLayout());
	JPanel contentPane4 = new JPanel(new BorderLayout());
	JPanel contentPane5 = new JPanel(new BorderLayout());
	JPanel contentPane6 = new JPanel(new BorderLayout());
	JPanel contentPane7 = new JPanel(new BorderLayout());
	JPanel contentPane8 = new JPanel(new BorderLayout());
	JPanel contentPane9 = new JPanel(new BorderLayout());
	
	
	JPanel sizePanel=new JPanel();//new GridLayout(2,3));
	JPanel newSizePanel=new JPanel(new BorderLayout());
	JPanel DpiPanel=new JPanel(new GridLayout(1,2));
	JPanel outputFormatPanel=new JPanel();//(new GridLayout(2,3));
	
	JPanel someStandardDimPanel=new JPanel();//(new GridLayout(2,3));
	JPanel colorTypePanel=new JPanel();//(new GridLayout(2,3));
	JPanel antialiasingPanel=new JPanel();//(new GridLayout(2,3));
	
	
	sizePanel.setBorder(BorderFactory.createCompoundBorder(BorderFactory.createTitledBorder("current size (in pixels)"),BorderFactory.createEmptyBorder(5,5,5,5)));
	newSizePanel.setBorder(BorderFactory.createCompoundBorder(BorderFactory.createTitledBorder("set size"),BorderFactory.createEmptyBorder(5,5,5,5)));
	DpiPanel.setBorder(BorderFactory.createCompoundBorder(BorderFactory.createTitledBorder("dots per inch"),BorderFactory.createEmptyBorder(5,5,5,5)));
	outputFormatPanel.setBorder(BorderFactory.createCompoundBorder(BorderFactory.createTitledBorder("select output format"),BorderFactory.createEmptyBorder(5,5,5,5)));
	someStandardDimPanel.setBorder(BorderFactory.createCompoundBorder(BorderFactory.createTitledBorder("some standard sizes (in pixels)"),BorderFactory.createEmptyBorder(5,5,5,5)));
	colorTypePanel.setBorder(BorderFactory.createCompoundBorder(BorderFactory.createTitledBorder("select color type"),BorderFactory.createEmptyBorder(5,5,5,5)));
	antialiasingPanel.setBorder(BorderFactory.createCompoundBorder(BorderFactory.createTitledBorder("select antialiasing quality"),BorderFactory.createEmptyBorder(5,5,5,5)));
	contentPane1l.setBorder(BorderFactory.createCompoundBorder(BorderFactory.createTitledBorder("resolution"),BorderFactory.createEmptyBorder(5,5,5,5)));
	contentPane1r.setBorder(BorderFactory.createCompoundBorder(BorderFactory.createTitledBorder("general settings"),BorderFactory.createEmptyBorder(5,5,5,5)));
	
	
	
	contentPane.setLayout(new BorderLayout());
	contentPane.add(contentPane1l,BorderLayout.CENTER);
	contentPane.add(contentPane1r,BorderLayout.EAST);
	contentPane1l.add(sizePanel, BorderLayout.NORTH);
	contentPane1l.add(contentPane2,BorderLayout.CENTER);
	contentPane2.add(newSizePanel, BorderLayout.NORTH);
	contentPane2.add(contentPane3,BorderLayout.CENTER);
	contentPane3.add(DpiPanel, BorderLayout.NORTH);
	contentPane3.add(contentPane4,BorderLayout.CENTER);
	contentPane4.add(someStandardDimPanel, BorderLayout.NORTH);
	//contentPane4.add(contentPane5,BorderLayout.CENTER);
	contentPane1r.add(colorTypePanel, BorderLayout.NORTH);
	contentPane1r.add(contentPane6,BorderLayout.CENTER);
	contentPane6.add(antialiasingPanel, BorderLayout.NORTH);
	contentPane6.add(contentPane7,BorderLayout.CENTER);
	contentPane7.add(outputFormatPanel, BorderLayout.NORTH);
	
	/*	contentPane7.add(contentPane8,BorderLayout.CENTER);
		contentPane8.add(outputFormatPanel, BorderLayout.NORTH);
		contentPane8.add(contentPane9,BorderLayout.CENTER);
		contentPane9.add(sizePanel, BorderLayout.NORTH);
	*///contentPane6.add(contentPane2,BorderLayout.CENTER);
	
	outputFormat=new JComboBox();
	
	outputFormat.insertItemAt("raytraced picture",0);
	
	
	if (true || surfex_.inAnApplet) {	    
	    outputFormat.insertItemAt("surf code",1);	
//	    outputFormat.insertItemAt("povray code",2);
	}
	outputFormat.setSelectedIndex(0);
	
	CL = new ChangeListener() {
		public void stateChanged(ChangeEvent ev) {
		    String command = ((AbstractButton) ev.getSource()).getText();
		    //         System.out.println(command );
		    int i;
		    /*	for (i = 0; i <= 7; i++) {
		    // System.out.println(command +" t"+ rb[i].getText() );
		    if (command.hashCode() == rb[i].getText().hashCode()
		    && rb[i].isSelected()) {
		    //System.out.println("text" );
		    width.setText(new String((new Integer(
		    aufloesungen[i][0])).toString()));
		    height.setText(new String((new Integer(
		    aufloesungen[i][1])).toString()));
		    aspectRatio.setSelected(false);
		    //width.disable();
		    // height.disable();
		    measure.setSelectedIndex(0);
		    updateLabel_newSize();
		    //raytracedPic.setSelected(true);
		    if (last[i] != rb[i].isSelected()) {
		    outputFormat.setSelectedIndex(0);//raytracedPic.setSelected(true);
		    for (j = 0; j <= 10; j++) {
		    last[j] = false;
		    }
		    last[i] = rb[i].isSelected();
		    }
		    }
		    }
		    
		    if (command.hashCode() == rb[8].getText().hashCode()
		    && rb[8].isSelected()) {
		    //      System.out.println("funktion nicht geschrieben ...");
		    //raytracedPic.setSelected(true);
		    if (last[8] != rb[8].isSelected()) {
		    outputFormat.setSelectedIndex(0);//raytracedPic.setSelected(true);
		    for (i = 0; i <= 10; i++) {
		    last[i] = false;
		    }
		    last[8] = rb[8].isSelected();
		    }
		    }
		    if (command.hashCode() == rb[9].getText().hashCode()
		    && rb[9].isSelected()) {
		    //      System.out.println("funktion nicht geschrieben ...");
		    //raytracedPic.setSelected(true);
		    if (last[9] != rb[9].isSelected()) {
						outputFormat.setSelectedIndex(0);//raytracedPic.setSelected(true);
						for (i = 0; i <= 10; i++) {
							last[i] = false;
						}
						last[9] = rb[9].isSelected();
					}
				}
				if (command.hashCode() == rb[10].getText().hashCode()
						&& rb[10].isSelected()) {
				//	/* width.enable();
				//	 raytracedPic.setSelected(true);
				//	 height.enable();
				//	 dpi.enable();
				//	 unitPixels.enable();
				//	 unitInches.enable();
				//	 unitCm.enable();
					//raytracedPic.setSelected(true);
					if (last[10] != rb[10].isSelected()) {
						outputFormat.setSelectedIndex(0);//raytracedPic.setSelected(true);
						for (i = 0; i <= 10; i++) {
							last[i] = false;
						}
						last[10] = rb[10].isSelected();
					}
				}
				if (command.hashCode() == unitPixels.getText().hashCode()
						&& unitPixels.isSelected()) {
					updateLabel_newSize();
					if (last[11] != unitPixels.isSelected()) {
						outputFormat.setSelectedIndex(0);//raytracedPic.setSelected(true);
						for (i = 11; i <= 13; i++) {
							last[i] = false;
						}
						last[11] = unitPixels.isSelected();
					}
					//raytracedPic.setSelected(true);
				}
				if (command.hashCode() == unitCm.getText().hashCode()
						&& unitCm.isSelected()) {
					if (!rb[10].isSelected()) {
						//unitPixels.setSelected(true);
						// besser:
						rb[10].setSelected(true);
					}
					if (last[12] != unitCm.isSelected()) {
						outputFormat.setSelectedIndex(0);
						for (i = 11; i <= 13; i++) {
							last[i] = false;
						}
						last[12] = unitCm.isSelected();
					}
					updateLabel_newSize();
					//outputFormat.setSelectedIndex(0)
				}
				if (command.hashCode() == unitInches.getText().hashCode()
						&& unitInches.isSelected()) {
					if (!rb[10].isSelected()) {
						// unitPixels.setSelected(true);
						// besser:
						rb[10].setSelected(true);
					}
					if (last[13] != unitInches.isSelected()) {
						outputFormat.setSelectedIndex(0);
						for (i = 11; i <= 13; i++) {
							last[i] = false;
						}
						last[13] = unitInches.isSelected();
					}
					updateLabel_newSize();
					//outputFormat.setSelectedIndex(0)
				}
				if (command.hashCode() == aspectRatio.getText().hashCode()) {
					if (aspectRatio.isSelected()) {
						height.setText(new Integer(Integer.parseInt(width
								.getText())
								* rayFrame.getContentPane().getSize().height
								/ rayFrame.getContentPane().getSize().width)
								.toString());
						for (i = 0; i <= 10; i++) {
							last[i] = false;
						}
						rb[10].setSelected(true);
						last[10] = true;
						updateLabel_newSize();
					}
					if (last[14] != aspectRatio.isSelected()) {
						outputFormat.setSelectedIndex(0);
						last[14] = aspectRatio.isSelected();
					}
				}
				if (command.hashCode() == ditheredCb.getText().hashCode()) {
					if (last[15] != ditheredCb.isSelected()) {
						outputFormat.setSelectedIndex(0);
						last[15] = ditheredCb.isSelected();
					}
				}
				if (command.hashCode() == outputFormat.getItemAt(1).toString().hashCode()) {
					for (i = 0; i <= 10; i++) {
						rb[10].setSelected(false);
					}
				}
				if (command.hashCode() == outputFormat.getItemAt(2).toString().hashCode()) {
					for (i = 0; i <= 10; i++) {
						rb[10].setSelected(false);
					}
				}

				// System.out.println("clicked" );
			*/
		}
		
	    };
	
	
// 	width = new JTextField(rayFrame.getContentPane().getSize().width+"");
// 	height = new JTextField(rayFrame.getContentPane().getSize().height+"");
	width = new JTextField("300");
	height = new JTextField("300");
	//	rb[10] = new JRadioButton("Set new size:", true);
	
	
	//	ButtonGroup group = new ButtonGroup();
	//	ButtonGroup standardDim = new ButtonGroup();
	
	// adding container
	//	panel.setLayout(new GridLayout(9, 2));
	//	panel.add(new JLabel("output format:"));
	
	outputFormatPanel.add(outputFormat);
	
/*		panel.add(surfCode);
		panel.add(new JLabel(""));
		
		panel.add(raytracedPic);
		panel.add(new JLabel(""));
*/
	
	colorType=new JComboBox();
	colorType.insertItemAt("color",0);
	colorType.insertItemAt("dithered - black/white",1);
// 	colorType.insertItemAt("stereo - red/green",2);
// 	colorType.insertItemAt("stereo - red/blue",3);
	colorType.setSelectedIndex(0);
	panel.add(new JLabel("color type:"));
	colorTypePanel.add(colorType);
	
	antialiasingQuality=new JComboBox();
	antialiasingQuality.insertItemAt("1",0);
	antialiasingQuality.insertItemAt("2",1);
	antialiasingQuality.insertItemAt("3",2);
	antialiasingQuality.insertItemAt("4",3);
	antialiasingQuality.insertItemAt("5",4);
	antialiasingQuality.insertItemAt("6",5);
	antialiasingQuality.setSelectedIndex(0);
//	panel.add(new JLabel("antialiasing level:"));
	antialiasingPanel.add(new JLabel("low(1) - high(6):"));
	antialiasingPanel.add(antialiasingQuality);
	
	panel.add(new JLabel("  Current size:"));
	sizePanel.add(newSize);//
	//new JLabel(rayFrame.getContentPane().getSize().width + " x "
	//		+ rayFrame.getContentPane().getSize().height));
	panel.add(new JLabel("  New size:"));
	
	JPanel p1 = new JPanel(new GridLayout(3, 2));
	newSizePanel.add(p1,BorderLayout.CENTER);
	
//	setNewSize(rayFrame.getContentPane().getSize().height,rayFrame.getContentPane().getSize().width);
	setNewSize(300,300);
	
	//	p1.add(newSize);
	//	p1.add(new JLabel(" "));
	
	//panel.add(rb[10]);
	//standardDim.add(rb[10]);
	//rb[10].addChangeListener(CL);
	
//		panel.add(new JLabel(""));
	
	
	p1.add(new JLabel("  width:"));
	width.addKeyListener(new KeyListener() {
		public void keyReleased(KeyEvent keyEvent) {
		    if (aspectRatio.isSelected()) {
			// musste mitm Dreisatz den height updaten
			setHeight(Double.valueOf(width.getText()).doubleValue()
				  * rayFrame.getContentPane().getSize().height
				  / rayFrame.getContentPane().getSize().width);
		    }
		    standardDim.setSelectedIndex(0);
		    //rb[10].setSelected(true);
		    updateLabel_newSize();
		    outputFormat.setSelectedIndex(0);
		}
		
		public void keyPressed(KeyEvent keyEvent) {
		}
		
		public void keyTyped(KeyEvent keyEvent) {
		}
	    });
	p1.add(width);
	
	//JPanel p2 = new JPanel(new GridLayout(1, 2));
	//panel.add(p2);
	p1.add(new JLabel("  height:"));
	height.addKeyListener(new KeyListener() {
		public void keyReleased(KeyEvent keyEvent) {
		    if (aspectRatio.isSelected()) {
			// musste mitm Dreisatz den height updaten
			setWidth((Double.valueOf(height.getText()).doubleValue()
				  * rayFrame.getContentPane().getSize().width
				  / rayFrame.getContentPane().getSize().height));
		    }
		    standardDim.setSelectedIndex(0);
		    outputFormat.setSelectedIndex(0);
		    //rb[10].setSelected(true);
		    updateLabel_newSize();
		}
		
		public void keyPressed(KeyEvent keyEvent) {
		}
		
		public void keyTyped(KeyEvent keyEvent) {
		}
	    });
	p1.add(height);
	
	
	
	
	JPanel p3 = new JPanel(new GridLayout(1, 2));
	DpiPanel.add(p3);
	p3.add(new JLabel("  dpi:"));
	dpi.addKeyListener(new KeyListener() {
		public void keyReleased(KeyEvent keyEvent) {
		    
		    updateLabel_newSize();
		    outputFormat.setSelectedIndex(0);
		}
		
		public void keyPressed(KeyEvent keyEvent) {
		}
		
		public void keyTyped(KeyEvent keyEvent) {
		}
	    });
	p3.add(dpi);
	
	measure=new JComboBox();
	
	measure.insertItemAt("pixel",0);
	measure.insertItemAt("cm",1);
	measure.insertItemAt("inch",2);
	
	measure.setSelectedIndex(0);
	oldMeasure = 0;
	
	measure.addActionListener(new ActionListener(){
		public void actionPerformed(ActionEvent ae)
		    {
// 		    setHeight(Double.valueOf(height.getText()).doubleValue());
// 		    setWidth(Double.valueOf(width.getText()).doubleValue());
// 		    if(measure.getSelectedIndex()==0){
// 			setNewSize(Double.valueOf(height.getText()).intValue(),
// 				   Double.valueOf(width.getText()).intValue());
// 		    }
// 		    if(measure.getSelectedIndex()==1){
// 			setNewSize((int)((Double.valueOf(dpi.getText()).intValue())*
// 					 (Double.valueOf(height.getText()).doubleValue())/2.4),
// 				   (int)((Double.valueOf(dpi.getText()).intValue())*
// 					 (Double.valueOf(width.getText()).doubleValue())/2.4));
// 		    }
// 		    if(measure.getSelectedIndex()==2){
// 			setNewSize((int)((Double.valueOf(dpi.getText()).intValue())*
// 					 (Double.valueOf(height.getText()).doubleValue())),
// 				   (int)((Double.valueOf(dpi.getText()).intValue())*
// 					 (Double.valueOf(width.getText()).doubleValue())));
// 		    }
// 		    setHeight(Double.valueOf(height.getText()).doubleValue());
// 		    setWidth(Double.valueOf(width.getText()).doubleValue());

		    if(measure.getSelectedIndex()==0){
			if(oldMeasure==0) {
			} 
			if(oldMeasure==1) {
			    setWidth((Double.valueOf(width.getText()).doubleValue()) * 
				     ((Double.valueOf(dpi.getText())).doubleValue())/2.54);
			    setHeight((Double.valueOf(height.getText()).doubleValue()) * 
				      ((Double.valueOf(dpi.getText())).doubleValue())/2.54);
			}
			if(oldMeasure==2) {
			    setWidth((Double.valueOf(width.getText()).doubleValue())*
				     ((Double.valueOf(dpi.getText())).doubleValue()));
			    setHeight((Double.valueOf(height.getText()).doubleValue())*
				      ((Double.valueOf(dpi.getText())).doubleValue()));
			}
			setNewSize((int)(Double.valueOf(height.getText()).doubleValue()),
				   (int)(Double.valueOf(width.getText()).doubleValue()));
		    }
		    if(measure.getSelectedIndex()==1){
			if(oldMeasure==0) {
			    setWidth((Double.valueOf(width.getText()).doubleValue()) / 
				     ((Double.valueOf(dpi.getText())).doubleValue())*2.54);
			    setHeight((Double.valueOf(height.getText()).doubleValue()) / 
				      ((Double.valueOf(dpi.getText()).doubleValue()))*2.54);
			} 
			if(oldMeasure==1) {
			}
			if(oldMeasure==2) {
			    setWidth((Double.valueOf(width.getText()).doubleValue())*2.54);
			    setHeight((Double.valueOf(height.getText()).doubleValue())*2.54);
			}
			setNewSize((int)((Double.valueOf(dpi.getText()).intValue())/2.54*
					 (Double.valueOf(height.getText()).doubleValue())),
				   (int)((Double.valueOf(dpi.getText()).intValue())/2.54*
					 (Double.valueOf(width.getText()).doubleValue())));
		    }
		    if(measure.getSelectedIndex()==2){
			if(oldMeasure==0) {
			    setWidth((Double.valueOf(width.getText()).doubleValue()) / 
				     ((Double.valueOf(dpi.getText())).doubleValue()));
			    setHeight((Double.valueOf(height.getText()).doubleValue()) / 
				      ((Double.valueOf(dpi.getText())).doubleValue()));
			} 
			if(oldMeasure==1) {
			    setWidth((Double.valueOf(width.getText()).doubleValue()) / 2.54);
			    setHeight((Double.valueOf(height.getText()).doubleValue()) / 2.54);
			}
			if(oldMeasure==2) {
			}
			setNewSize((int)((Double.valueOf(dpi.getText()).intValue())*
					 (Double.valueOf(height.getText()).doubleValue())),
				   (int)((Double.valueOf(dpi.getText()).intValue())*
					 (Double.valueOf(width.getText()).doubleValue())));
		    }
		    oldMeasure = measure.getSelectedIndex();
		}
	    });
	
	
	//JPanel p35 = new JPanel(new GridLayout(1, 2));
	//panel.add(p35);
	p1.add(new JLabel("Unit:"));
	p1.add(measure);
/*
  JPanel p4 = new JPanel(new GridLayout(1, 2));
  panel.add(p4);
  p4.add(new JLabel("  Units:"));
  ButtonGroup units = new ButtonGroup();
  units.add(unitPixels);
  unitPixels.addChangeListener(CL);
  p4.add(unitPixels);
  
  panel.add(new JLabel(""));
  
  JPanel p5 = new JPanel(new GridLayout(1, 2));
  panel.add(p5);
  units.add(unitCm);
		unitCm.addChangeListener(CL);
		p5.add(new JLabel(""));
		p5.add(unitCm);

		panel.add(new JLabel(""));

		JPanel p6 = new JPanel(new GridLayout(1, 2));
		panel.add(p6);
		units.add(unitInches);
		unitInches.addChangeListener(CL);
		p6.add(new JLabel(""));
		p6.add(unitInches);
*/
//	newSizePanel.add(aspectRatio,BorderLayout.SOUTH);
	aspectRatio.addChangeListener(CL);
	//	panel.add(new JLabel("(raytraced preview)"));
	
	
	standardDim=new JComboBox();
	standardDim.insertItemAt("choose a size",0);	
 	standardDim.insertItemAt("for presentation (600x600)",1);
 	standardDim.insertItemAt("for web (300x300)",2);
	standardDim.insertItemAt("240 x 240",3);
	standardDim.insertItemAt("300 x 300",4);
	standardDim.insertItemAt("360 x 360",5);
	standardDim.insertItemAt("480 x 480",6);
	standardDim.insertItemAt("600 x 600",7);
	standardDim.insertItemAt("728 x 728",8);
	standardDim.insertItemAt("900 x 900",9);
	standardDim.insertItemAt("1024 x 1024",10);
	standardDim.insertItemAt("1200 x 1200",11);
// 	standardDim.insertItemAt("Best fit to desktop",1);
// 	standardDim.insertItemAt("screen size",2);

	
	standardDim.addActionListener(new ActionListener(){
		public void actionPerformed(ActionEvent ae){
		    int i=standardDim.getSelectedIndex();
		    switch (i){
			case 0:break;//nix passiert;
			case 1:updateSize(600,600);break;//updateSize(h,w);
			case 2:updateSize(300,300);break;//updateSize(,);
			case 3:updateSize(240,240);break;
			case 4:updateSize(300,300);break;
			case 5:updateSize(360,360);break;
			case 6:updateSize(480,480);break;
			case 7:updateSize(600,600);break;
			case 8:updateSize(728,728);break;
			case 9:updateSize(900,900);break;
			case 10:updateSize(1024,1024);break;
			case 11:updateSize(1200,1200);break;
		    }
		}
	    });
	
	standardDim.setSelectedIndex(0);
	
	//panel.add(new JLabel("some standard dimensions:"));
	someStandardDimPanel.add(standardDim);
	
	
/*		JPanel panel2 = new JPanel(new GridLayout(12, 3));
		
panel2.add(new JLabel());
panel2.add(new JLabel());
panel2.add(new JLabel());
panel2.add(new JLabel());
panel2.add(new JLabel());
		panel2.add(new JLabel());
		panel2.add(new JLabel());
		panel2.add(new JLabel());
		panel2.add(new JLabel());
		panel2.add(new JLabel());
		panel2.add(new JLabel());
		panel2.add(new JLabel());
		panel2.add(new JLabel());
		panel2.add(new JLabel("Some standard dimensions:"));
		panel2.add(new JLabel());

		rb[8] = new JRadioButton("Best fit to desktop");
		rb[8].addChangeListener(CL);
		rb[9] = new JRadioButton("Screen size");
		rb[9].addChangeListener(CL);

		for (i = 0; i <= 7; i++) {
			if (i % 2 == 0) {
				panel2.add(new JLabel());
			}
			rb[i] = new JRadioButton(aufloesungen[i][0] + " x "
					+ aufloesungen[i][1]);
			panel2.add(rb[i]);
		//	standardDim.add(rb[i]);
			rb[i].addChangeListener(CL);
		}
		panel2.add(new JLabel());
		panel2.add(rb[8]);
		standardDim.add(rb[8]);
		panel2.add(new JLabel());
		panel2.add(new JLabel());
		panel2.add(rb[9]);
		standardDim.add(rb[9]);

		panel2.add(new JLabel());

		contentPane.add(panel2, BorderLayout.EAST);
*/
	JPanel panel3 = new JPanel(new FlowLayout());
	
	ActionListener AL = new ActionListener() {
		public void actionPerformed(ActionEvent evt) {
		    String command = ((JButton) evt.getSource()).getText();
		    if (command.hashCode() == SaveButton.getText().hashCode()) {
			//      System.out.println("saveButton!");
			setVisible(false);
			
			// creating new Frame
			final JFrame frame2 = new JFrame("save picture"
							 + projectName);
			
			// adding container
			Container contentPane = frame2.getContentPane();
			
			if (surfex_.inAnApplet) {
			    ActionListener aL = new ActionListener() {
				    public void actionPerformed(ActionEvent actionEvent) {
					String command2 = actionEvent.getActionCommand();
					//          System.out.println("com:"+command2);
					// check if doubleclickt or if "open" was pressed
					if (command2.equals("create and save as")) {
					    // creating complete filepath
					    String filelocation = tf.getText();
					    //              System.out.println("file:"+filelocation);
					    // ausgabe fehler bein z.B.: d:\ -> d:\\ bzw \ -> \\
					    //          System.out.println("filelocation : "+ filelocation + "\n");
					    if (outputFormat.getSelectedIndex()==2) {  // povcode
						project.saveFile(
						    filelocation,
						    1,
						    (colorType.getSelectedIndex()==1),//dithered?
						    (antialiasingQuality.getSelectedIndex()!=0)
						    ,heightvalue,
						    widthvalue,
						    Integer.parseInt(dpi
								     .getText()),surfex_.jv4sx);
					    } else {
						if (outputFormat.getSelectedIndex()==1) {  // surfcode
						    project.saveFile(filelocation, 2,
								     (colorType.getSelectedIndex()==1),//dithered?
								     (antialiasingQuality.getSelectedIndex()!=0)
								     ,
								     heightvalue,
								     widthvalue,
								     Integer.parseInt(dpi
										      .getText()),surfex_.jv4sx);
						} else {
						    // raytraced pic
						    project.saveFile(filelocation, 3,
								     (colorType.getSelectedIndex()==1),//dithered?
								     (antialiasingQuality.getSelectedIndex()!=0),
								     heightvalue,widthvalue,		
								     Integer.parseInt(dpi.getText()),surfex_.jv4sx);
						}
					    }
					    //              saveFile(filelocation);
					    frame2.dispose();
					} else {
					    //if(command.equals(JFileChooser.CANCEL_SELECTION)){
					    frame2.dispose();
					    //}
					}
				    }
				};
			    contentPane.setLayout(new BorderLayout());
			    contentPane.add(new JLabel("Give filename: "),
					    BorderLayout.WEST);
			    if ((colorType.getSelectedIndex()==1)) {  // dithered!
				tf = new JTextField("test.tif");
			    } else {
				tf = new JTextField("test.jpg");
			    }
			    contentPane.add(tf, BorderLayout.CENTER);
			    JPanel tmpPanel = new JPanel();
			    tmpPanel.setLayout(new FlowLayout());
			    if ((colorType.getSelectedIndex()==1)) {  // dithered!
				String strFormats[] = { "tif" };
				tmpPanel.add(new JComboBox(strFormats));
			    } else {
				String strFormats[] = { "jpg" };
				tmpPanel.add(new JComboBox(strFormats));
			    }
			    JButton jbsave = new JButton("create and save as");
			    jbsave.addActionListener(aL);
			    tmpPanel.add(jbsave);
			    JButton jbcancel = new JButton("  cancel  ");
			    jbcancel.addActionListener(aL);
			    tmpPanel.add(jbcancel);
			    contentPane.add(tmpPanel, BorderLayout.EAST);
			    frame2.setSize(500, 75);
			} else { // i.e.: not inAnApplet
//			    System.out.println("filechooser...");
			    // creating Filechooser
			    JFileChooser fileChooser = new JFileChooser(
				surfex_.currentDirectory);
//			    System.out.println("saveDialogType...");
			    fileChooser.setDialogType(JFileChooser.SAVE_DIALOG);
//			    System.out.println("saveDialogType");
			    // adding filechooser
			    contentPane.add(fileChooser, BorderLayout.CENTER);
			    
			    // Create ActionListener
			    ActionListener actionListener = new ActionListener() {
				    public void actionPerformed(ActionEvent actionEvent) {
					JFileChooser theFileChooser = (JFileChooser) actionEvent
					    .getSource();
					
					// get command2
					String command2 = actionEvent
					    .getActionCommand();
					
					// check if doubleclickt or if "open" was pressed
					if (command2
					    .equals(JFileChooser.APPROVE_SELECTION)) {
					    File selectedFile = theFileChooser
						.getSelectedFile();
//					    System.out.println("file:"+selectedFile);
//					    System.out.println(" dir:"+theFileChooser.getCurrentDirectory());
					    surfex_.currentDirectory = theFileChooser.getCurrentDirectory().getAbsolutePath();
					    // making savepopup invisible/removing it
					    frame2.dispose();
					    
					    // creating complete filepath
					    String filelocation = new String(
						selectedFile.getParent()
							+ File.separator
						+ selectedFile.getName());
					    
					    // ausgabe fehler bein z.B.: d:\ -> d:\\ bzw \ -> \\
					    //          System.out.println("filelocation : "+ filelocation + "\n");
					    if (outputFormat.getSelectedIndex()==2) {
						//povcode
						project.saveFile(
						    filelocation,
						    1,
						    (colorType.getSelectedIndex()==1),//dithered?
						    (antialiasingQuality.getSelectedIndex()!=0)
						    ,
						    heightvalue,widthvalue,
						    Integer.parseInt(dpi
								     .getText()),surfex_.jv4sx);
					    } else {
						if (outputFormat.getSelectedIndex()==1) {
						    //surfcode
						    project.saveFile(filelocation, 2,(colorType.getSelectedIndex()==1),//dithered?
								     (antialiasingQuality.getSelectedIndex()!=0)
								     ,
								     heightvalue,widthvalue,
								     Integer.parseInt(dpi
										      .getText()),surfex_.jv4sx);
						} else {
						    //                System.out.println("saveFile...");
						    project.saveFile(filelocation, 3,(colorType.getSelectedIndex()==1),//dithered?
								     (antialiasingQuality.getSelectedIndex()!=0)
								     ,
								     heightvalue,widthvalue,
								     Integer.parseInt(dpi
										      .getText()),surfex_.jv4sx);
						}
					    }
					    //              saveFile(filelocation);
					} else {
					    //if(command.equals(JFileChooser.CANCEL_SELECTION)){
					    frame2.setVisible(false);
					    //}
					}
				    }
				};
			    
			    if (outputFormat.getSelectedIndex()==2) {
				// povcode
				FileFilter acfFilter = new ExtensionFileFilter(
				    "all common files", new String[] { "pov" });
				fileChooser.addChoosableFileFilter(acfFilter);
				FileFilter spcFilter = new ExtensionFileFilter(
				    "*.pov (pov-file)", new String[] { "pov" });
				fileChooser.addChoosableFileFilter(spcFilter);
				fileChooser.setFileFilter(spcFilter);
				try {
				    File lastFile = new File(project.filename);
				    String theName = lastFile.getName();
				    fileChooser.setSelectedFile( 
					new File(surfex_.changeFilenameExtension(theName,".pov")));
//	    System.out.println("name:"+theName);
				} catch(Exception fileEx) {
				    System.out.println("fileEx:"+fileEx.toString());
				}					
			    } else {
				if (outputFormat.getSelectedIndex()==1) {
				    //surfcode
				    FileFilter acfFilter = new ExtensionFileFilter(
					"all common files",
					new String[] { "surf", "pic" });
				    fileChooser.addChoosableFileFilter(acfFilter);
				    FileFilter spcFilter = new ExtensionFileFilter(
					"*.surf *.pic (surf-file)",
					new String[] { "surf", "pic" });
				    fileChooser.addChoosableFileFilter(spcFilter);
				    fileChooser.setFileFilter(spcFilter);
				    try {
					File lastFile = new File(project.filename);
					String theName = lastFile.getName();
					fileChooser.setSelectedFile( 
					    new File(surfex_.changeFilenameExtension(theName,".pic")));
//	    System.out.println("name:"+theName);
				    } catch(Exception fileEx) {
					System.out.println("fileEx:"+fileEx.toString());
				    }					    
				} else {
				    if (outputFormat.getSelectedIndex()==0) {
					// raytraced pic 
					if ((colorType.getSelectedIndex()==1)) { // dithered!
					    FileFilter acfFilter = new ExtensionFileFilter(
						"all common files",
						new String[] { "tif", "ps", "eps", 
							       "xbm", "pgm", "pbm" });
					    fileChooser.addChoosableFileFilter(acfFilter);
					    FileFilter spcFilter;
					    spcFilter = new ExtensionFileFilter(
						"TIFF (*.tif)", new String[] { "tif" });
					    fileChooser.addChoosableFileFilter(spcFilter);

					    spcFilter = new ExtensionFileFilter(
						"Postscript (*.ps)", new String[] { "ps" });
					    fileChooser.addChoosableFileFilter(spcFilter);

					    FileFilter epsFilter = new ExtensionFileFilter(
						"Enc. Postscript (*.eps)", new String[] { "eps" });
					    fileChooser.addChoosableFileFilter(epsFilter);

					    spcFilter = new ExtensionFileFilter(
						"XBM (*.xbm)", new String[] { "xbm" });
					    fileChooser.addChoosableFileFilter(spcFilter);

					    spcFilter = new ExtensionFileFilter(
						"PGM (*.pgm)", new String[] { "pgm" });
					    fileChooser.addChoosableFileFilter(spcFilter);

					    spcFilter = new ExtensionFileFilter(
						"PBM (*.pbm)", new String[] { "pbm" });
					    fileChooser.addChoosableFileFilter(spcFilter);

					    fileChooser.setFileFilter(epsFilter);
					    try {
						File lastFile = new File(project.filename);
						String theName = lastFile.getName();
						fileChooser.setSelectedFile( 
						    new File(surfex_.changeFilenameExtension(theName,".eps")));
//	    System.out.println("name:"+theName);
					    } catch(Exception fileEx) {
						System.out.println("fileEx:"+fileEx.toString());
					    }					    

// 										spcFilter = new ExtensionFileFilter(
// 												"*.eps", new String[] { "eps" });
//										fileChooser.addChoosableFileFilter(spcFilter);
					} else {
					    FileFilter acfFilter = new ExtensionFileFilter(
						"all common files",
						new String[] { "jpg", "ras", "ppm", "xwd" });
					    fileChooser
						.addChoosableFileFilter(acfFilter);
					    FileFilter spcFilter;
					    spcFilter = new ExtensionFileFilter(
						"*.png", new String[] { "png" });
//					    fileChooser.addChoosableFileFilter(spcFilter);
					    spcFilter = new ExtensionFileFilter(
						"*.bmp", new String[] { "bmp" });
					    //              fileChooser.addChoosableFileFilter(spcFilter);
					    spcFilter = new ExtensionFileFilter(
						"*.gif", new String[] { "gif" });
					    //              fileChooser.addChoosableFileFilter(spcFilter);
					    
					    spcFilter = new ExtensionFileFilter(
						"*.ppm", new String[] { "ppm" });
//					    fileChooser.addChoosableFileFilter(spcFilter);
					    spcFilter = new ExtensionFileFilter(
						"*.xwd", new String[] { "xwd" });
//					    fileChooser.addChoosableFileFilter(spcFilter);
					    spcFilter = new ExtensionFileFilter(
						"tiff (*.tif)", new String[] { "tif" });
					    //              fileChooser.addChoosableFileFilter(spcFilter);
					    spcFilter = new ExtensionFileFilter(
						"*.ras", new String[] { "ras" });
//					    fileChooser.addChoosableFileFilter(spcFilter);
					    FileFilter jpgFilter = new ExtensionFileFilter(
						"JPEG (*.jpg)", new String[] { "jpg" });
					    fileChooser.addChoosableFileFilter(jpgFilter);
					    spcFilter = new ExtensionFileFilter(
						"sun rasterfile (*.ras)", new String[] { "ras" });
					    fileChooser.addChoosableFileFilter(spcFilter);
					    spcFilter = new ExtensionFileFilter(
						"PPM (*.ppm)", new String[] { "ppm" });
					    fileChooser.addChoosableFileFilter(spcFilter);
					    spcFilter = new ExtensionFileFilter(
						"XWD (*.xwd)", new String[] { "xwd" });
					    fileChooser.addChoosableFileFilter(spcFilter);

					    fileChooser.setFileFilter(jpgFilter);
					    try {
						File lastFile = new File(project.filename);
						String theName = lastFile.getName();
						fileChooser.setSelectedFile( 
						    new File(surfex_.changeFilenameExtension(theName,".jpg")));
//	    System.out.println("name:"+theName);
					    } catch(Exception fileEx) {
						System.out.println("fileEx:"+fileEx.toString());
					    }
					}
				    }
				}
				
			    }
			    
			    // adding actionListener
			    fileChooser.addActionListener(actionListener);
			    frame2.pack();
			} // end of else (i.e. !inAnApplet)
			
			frame2.setVisible(true);
			
			//oeffen den Kram unter filename;
			
			// end: command == "load"
		    }
		    if (command.hashCode() == DefaultButton.getText().hashCode()) {
			//setVisible(false);
//			setNewSize(rayFrame.getContentPane().getSize().height,rayFrame.getContentPane().getSize().width);
			setNewSize(300, 300);
			//	rb[10].setSelected(true);
			dpi.setText("300");
// 			setHeight(rayFrame.getContentPane().getSize().height);
// 			setWidth(rayFrame.getContentPane().getSize().width);
			//ditheredCb.setSelected(false);
			colorType.setSelectedIndex(1);// color
			//antialiasing.setSelected(true);
			antialiasingQuality.setSelectedIndex(2);// true
			aspectRatio.setSelected(true);
			outputFormat.setSelectedIndex(0);
			//for (i = 0; i <= 15; i++) {
			//	last[i] = false;
			//}
			//last[0] = true;
			//last[11] = true;
			//last[14] = true;
			
			standardDim.setSelectedIndex(0);
			measure.setSelectedIndex(0);//pixel
			
			updateLabel_newSize();
		    }
		    if (command.hashCode() == CancelButton.getText().hashCode()) {
			setVisible(false);
		    }
		}
	    };
	
	//panel3.add(new JLabel());
	SaveButton.addActionListener(AL);
	SaveButton.setDefaultCapable(true);
	panel3.add(SaveButton);
	CancelButton.addActionListener(AL);
	panel3.add(CancelButton);
	DefaultButton.addActionListener(AL);
	panel3.add(DefaultButton);
	//panel3.add(new JLabel());
	
	contentPane.add(panel3, BorderLayout.SOUTH);
/*
  group.add(raytracedPic);
  group.add(povCode);
  group.add(surfCode);
  
*/	
	this.pack();
	this.setLocation(200,100);
	setVisible(false);
    }
    
    //     void saveFile(String filelocation) {
    //  project.saveFile(filelocation);
    //     } // end of saveFile(String filelocation)
    
    public void updateLabel_newSize() {
	if (width.getText() != "" && height.getText() != ""
	    && dpi.getText() != "") {
	    if (measure.getSelectedIndex()==0) {
		// pixel selektiert
		setNewSize(Double.valueOf(height.getText()).intValue(),
			   Double.valueOf(width.getText()).intValue());
	    }
	    if (measure.getSelectedIndex()==2) {
		// inch selektiert
		
		setNewSize((int)(Double.parseDouble(height.getText()) * Integer
				 .parseInt(dpi.getText())),(int)(Double.parseDouble(width.getText()) * Integer
								 .parseInt(dpi.getText())));
	    }
	    if (measure.getSelectedIndex()==1) {
		// cm selektiert
		setNewSize((int)(Math.round((Double.parseDouble(height.getText())
					     * Integer.parseInt(dpi.getText()) * 0.3937))),(int)(Math.round((Double.parseDouble(width.getText())
													     * Integer.parseInt(dpi.getText()) * 0.3937))));
	    }
	}
	// SwingUtilities.updateComponentTreeUI(this);
    }
    
    public void updateSize(int h,int w){
	setNewSize(h,w);
	if(measure.getSelectedIndex()==0){
	    setWidth(h);
	    setHeight(w);
	}
	if(measure.getSelectedIndex()==1){
	    setWidth(((w/(Double.valueOf(dpi.getText()).doubleValue())*2.4)));
	    setHeight(((h/(Double.valueOf(dpi.getText()).doubleValue())*2.4)));
	}
	if(measure.getSelectedIndex()==2){
	    setWidth(w/(Double.valueOf(dpi.getText()).doubleValue()));
	    setHeight(h/(Double.valueOf(dpi.getText()).doubleValue()));
	}
	
    }
    
    public void setNewSize(int h, int w){
	heightvalue=h;
	widthvalue=w;
	newSize.setText(h+" x "+w);
    }
    
    public void setHeight(double h){
	if(measure.getSelectedIndex()==0){
	    //pixel ist selektiert, d.h. keine Nachkommastellen anzeigen!	    
	    height.setText(((int)h)+"");
	} else {
	    // angabe in cm / inch:
	    // bis zu acht stellen (mit komma ) anzeigen:
	    height.setText((h+"        ").substring(0,8).replaceAll(" ",""));	    
	}		
    }

    public void setWidth(double w){
	if(measure.getSelectedIndex()==0){
	    //pixel ist selektiert, d.h. keine Nachkommastellen anzeigen!
	    width.setText(((int)w)+"");
	}else{
	    // angabe in cm / inch:
	    // bieigen:
	    width.setText((w+"        ").substring(0,8).replaceAll(" ",""));
	}
    }
    
} // end class SavePicDialog

