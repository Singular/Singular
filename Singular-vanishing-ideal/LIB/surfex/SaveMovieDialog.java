////////////////////////////////////////////////////////////////////////
//
// This file SaveMovieDialog.java is part of SURFEX.
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
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import javax.swing.event.ListSelectionEvent;
import javax.swing.filechooser.FileFilter;
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
// class SaveMovieDialog
//
//////////////////////////////////////////////////////////////

public class SaveMovieDialog extends JFrame {
    JTextField tf;
    
    RayFrame rayFrame;
    //JComboBox outputFormat;
    
    JComboBox colorType;

    JComboBox omitType;
    
    JComboBox antialiasingQuality;
    
    JComboBox standardDim;
    
    
    JSpinner XrotSpinner =new JSpinner(new SpinnerNumberModel(1,0,5,1));
    JSpinner YrotSpinner =new JSpinner(new SpinnerNumberModel(2,0,5,1));
    JSpinner ZrotSpinner =new JSpinner(new SpinnerNumberModel(0,0,5,1));
    JSpinner FramesPerSecond=new JSpinner(new SpinnerNumberModel(5,1,60,1));
    
    JScrollPane listScrollPane;
    
    private JList parameterList;
    private DefaultListModel listModel;
    
    private static final String addString = "add";
    private static final String removeString = "remove from list";
    private JLabel addLabel;
    private JButton removeButton;
    private JComboBox paramName;
    
    
    JRadioButton ROTATE = new JRadioButton("ROTATE_Y_AXIS");
    
    
    JComboBox measure;
    
    JPanel panel = new JPanel();/*
				  
//		JRadioButton povCode = new JRadioButton("PovRay code");

//JRadioButton surfCode = new JRadioButton("Surf code");

//JRadioButton raytracedPic = new JRadioButton("Raytraced pic", true);

//JRadioButton rb[] = new JRadioButton[11];

//int aufloesungen[][] = { { 300, 300 }, { 600, 600 }, { 900, 900 },
//		{ 1200, 1200 }, { 240, 240 }, { 360, 360 }, { 480, 480 },
//		{ 640, 640 } };

				*/	ChangeListener CL;
    
    //boolean last[] = { true, false, false, false, false, false, false, false,
    //		false, false, false, true, false, false, true, false };
    
    //boolean actionWasPerformed = false;
    
    JTextField width;
    
    JTextField height;
    
    int heightvalue=0;
    int widthvalue=0;
    
    JCheckBox aspectRatio = new JCheckBox("Preserve aspect ratio", false);
    
    //JRadioButton unitPixels = new JRadioButton("Pixels", true);
    
    JTextField dpi = new JTextField("300");
    
    //JRadioButton unitCm = new JRadioButton("cm");
    
    //JRadioButton unitInches = new JRadioButton("inches");
    
    //int i, j;
    
    //JCheckBox ditheredCb = new JCheckBox("dithered - black/white", false);
    
    //public JCheckBox antialiasing = new JCheckBox("antialiasing", false);
    
    JButton SaveButton = new JButton("create and save as");
    
    boolean actionWasPerformed = false;
    
    
    JButton CancelButton = new JButton("cancel");
    
    JButton DefaultButton = new JButton("default");
    
    JLabel newSize = new JLabel();
    
    JTextField Time = new JTextField("10");
    
    
    Project project;
    
    surfex surfex_;
    
    String projectName;
    
    SaveMovieDialog(String proName, RayFrame ray, Project pro, surfex su) {
	super("save movie " + proName);
	surfex_ = su;
	rayFrame = ray;
	project = pro;
	this.projectName = proName;
	//setSize(790, 400);
	
	JPanel contentPane = (JPanel) getContentPane();
	JPanel contentPane1l = new JPanel(new BorderLayout());
	JPanel contentPane1l2 = new JPanel(new BorderLayout());
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
	//JPanel DpiPanel=new JPanel(new GridLayout(1,2));
	JPanel fpsPanel=new JPanel(new GridLayout(1,2));
	JPanel lengthPanel=new JPanel(new GridLayout(1,2));
	JPanel omitTypePanel=new JPanel();
	
	JPanel someStandardDimPanel=new JPanel();//(new GridLayout(2,3));
	JPanel colorTypePanel=new JPanel();//(new GridLayout(2,3));
	JPanel antialiasingPanel=new JPanel();//(new GridLayout(2,3));
	
	JPanel paramPanel=new JPanel(new BorderLayout());//(new GridLayout(2,3));
	JPanel paramAddPanel=new JPanel();//(new GridLayout(2,3));
	JPanel rotatePanel=new JPanel(new GridLayout(1,6));
	
	
	sizePanel.setBorder(BorderFactory.createCompoundBorder(BorderFactory.createTitledBorder("current size"),BorderFactory.createEmptyBorder(5,5,5,5)));
	newSizePanel.setBorder(BorderFactory.createCompoundBorder(BorderFactory.createTitledBorder("set size"),BorderFactory.createEmptyBorder(5,5,5,5)));
	//DpiPanel.setBorder(BorderFactory.createCompoundBorder(BorderFactory.createTitledBorder("Dpi"),BorderFactory.createEmptyBorder(5,5,5,5)));
	fpsPanel.setBorder(BorderFactory.createCompoundBorder(BorderFactory.createTitledBorder("smoothness"),BorderFactory.createEmptyBorder(5,5,5,5)));
	lengthPanel.setBorder(BorderFactory.createCompoundBorder(BorderFactory.createTitledBorder("length"),BorderFactory.createEmptyBorder(5,5,5,5)));
	omitTypePanel.setBorder(BorderFactory.createCompoundBorder(BorderFactory.createTitledBorder("omit selected frames"),BorderFactory.createEmptyBorder(5,5,5,5)));
	someStandardDimPanel.setBorder(BorderFactory.createCompoundBorder(BorderFactory.createTitledBorder("some standard sizes (in pixels):"),BorderFactory.createEmptyBorder(5,5,5,5)));
	colorTypePanel.setBorder(BorderFactory.createCompoundBorder(BorderFactory.createTitledBorder("select color type"),BorderFactory.createEmptyBorder(5,5,5,5)));
	antialiasingPanel.setBorder(BorderFactory.createCompoundBorder(BorderFactory.createTitledBorder("select antialiasing quality"),BorderFactory.createEmptyBorder(5,5,5,5)));
	paramPanel.setBorder(BorderFactory.createCompoundBorder(BorderFactory.createTitledBorder("parameters to run"),BorderFactory.createEmptyBorder(5,5,5,5)));
	paramAddPanel.setBorder(BorderFactory.createCompoundBorder(BorderFactory.createTitledBorder("add parameters"),BorderFactory.createEmptyBorder(5,5,5,5)));
	rotatePanel.setBorder(BorderFactory.createCompoundBorder(BorderFactory.createTitledBorder("rotations"),BorderFactory.createEmptyBorder(5,5,5,5)));
	contentPane1l.setBorder(BorderFactory.createCompoundBorder(BorderFactory.createTitledBorder("resolution"),BorderFactory.createEmptyBorder(5,5,5,5)));
	contentPane1l2.setBorder(BorderFactory.createCompoundBorder(BorderFactory.createTitledBorder("frames"),BorderFactory.createEmptyBorder(5,5,5,5)));
	contentPane1r.setBorder(BorderFactory.createCompoundBorder(BorderFactory.createTitledBorder("general settings"),BorderFactory.createEmptyBorder(5,5,5,5)));
	

	JPanel paneLeft = new JPanel(new BorderLayout());
	
	contentPane.setLayout(new BorderLayout());
	paneLeft.add(contentPane1l,BorderLayout.NORTH);
	paneLeft.add(contentPane1l2,BorderLayout.SOUTH);
	contentPane.add(paneLeft,BorderLayout.CENTER);
	contentPane.add(contentPane1r,BorderLayout.EAST);
	contentPane1l.add(sizePanel, BorderLayout.NORTH);
	contentPane1l.add(contentPane2,BorderLayout.CENTER);
	contentPane2.add(newSizePanel, BorderLayout.NORTH);
	contentPane2.add(contentPane3,BorderLayout.CENTER);
	contentPane3.add(contentPane4,BorderLayout.CENTER);

	// frames:
	contentPane1l2.add(fpsPanel, BorderLayout.NORTH);
	contentPane1l2.add(lengthPanel, BorderLayout.CENTER);
	contentPane1l2.add(omitTypePanel, BorderLayout.SOUTH);

	//
	contentPane4.add(contentPane5,BorderLayout.CENTER);
	contentPane5.add(someStandardDimPanel, BorderLayout.NORTH);
	//contentPane4.add(contentPane5,BorderLayout.CENTER);
	contentPane1r.add(colorTypePanel, BorderLayout.NORTH);
	contentPane1r.add(contentPane6,BorderLayout.CENTER);
	contentPane6.add(antialiasingPanel, BorderLayout.NORTH);
	contentPane6.add(contentPane7,BorderLayout.CENTER);
	contentPane7.add(paramPanel, BorderLayout.NORTH);
	
	contentPane7.add(contentPane8,BorderLayout.CENTER);
	contentPane8.add(paramAddPanel, BorderLayout.NORTH);
	contentPane8.add(contentPane9,BorderLayout.CENTER);
	contentPane9.add(rotatePanel, BorderLayout.NORTH);
	//*///contentPane6.add(contentPane2,BorderLayout.CENTER);
	
	//outputFormat=new JComboBox();
	
	//outputFormat.insertItemAt("raytraced picture",0);
	
	if (true || surfex_.inAnApplet) {
	    
	    //		outputFormat.insertItemAt("povray code",1);
	    //	outputFormat.insertItemAt("surf code",2);	
	}
	//outputFormat.setSelectedIndex(0);
	
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
	
	//outputFormatPanel.add(outputFormat);
	
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

	omitType=new JComboBox();
	omitType.insertItemAt("omit no frame",0);
	omitType.insertItemAt("omit first frame",1);
	omitType.insertItemAt("omit last frame",2);
	omitType.setSelectedIndex(0);
	omitTypePanel.add(omitType);
	
	antialiasingQuality=new JComboBox();
	antialiasingQuality.insertItemAt("1",0);
	antialiasingQuality.insertItemAt("2",1);
	antialiasingQuality.insertItemAt("3",2);
	antialiasingQuality.insertItemAt("4",3);
	antialiasingQuality.insertItemAt("5",4);
	antialiasingQuality.insertItemAt("6",5);
	antialiasingQuality.setSelectedIndex(0);
	panel.add(new JLabel("antialiasing level:"));
	antialiasingPanel.add(antialiasingQuality);
	
	paramPanel.add(new JLabel());
	// p6N.add(new JLabel("Parameters to run:"));
	// p6N.add(new JLabel());
	
	// p6C.add(new JLabel());
	listModel = new DefaultListModel();
	
	//Create the list and put it in a scroll pane.
	parameterList = new JList(listModel);
	parameterList.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
	parameterList.setSelectedIndex(0);
	
	// parameterList.addListSelectionListener(this);
	parameterList.setVisibleRowCount(2);
	listScrollPane = new JScrollPane(parameterList);
	
	//   addLabel = new JLabel("Add Params to List:");
	
	
	
////////////////////////	//
	//addButton .... in JLabel aendern
	
	
	
	
	addListener addListener = new addListener(addLabel);
	//  addButton.setActionCommand(addString);
	//  addButton.addActionListener(addListener);
	//  addButton.setEnabled(false);
	
	removeButton = new JButton(removeString);
	removeButton.setActionCommand(removeString);
	removeButton.addActionListener(new removeListener());
	removeButton.setEnabled(false);
	
	paramName = pro.parAdmin.paramComboBox;
	paramName.addActionListener(addListener);
	// paramName.setEnabled(false);
	//  paramName.getDocument().addDocumentListener(addListener);
	//String name = listModel.getElementAt(
	//                    parameterList.getSelectedIndex()).toString();
	
	
	
	paramPanel.add(listScrollPane,BorderLayout.CENTER);
	paramPanel.add(removeButton,BorderLayout.SOUTH);
	// p6S.add(new JLabel());
	// p6S.add(addLabel);
	paramAddPanel.add(paramName);
	
	rotatePanel.add(new JLabel("#x-rot"));
	//XrotSpinner.set
	rotatePanel.add(XrotSpinner);
	rotatePanel.add(new JLabel("#y-rot"));
	rotatePanel.add(YrotSpinner);
	rotatePanel.add(new JLabel("#z-rot"));
	rotatePanel.add(ZrotSpinner);
	
	
	panel.add(new JLabel("  Current size:"));
	sizePanel.add(newSize);//
	//new JLabel(rayFrame.getContentPane().getSize().width + " x "
	//		+ rayFrame.getContentPane().getSize().height));
	panel.add(new JLabel("  New size:"));
	
	JPanel p1 = new JPanel(new GridLayout(3, 2));
	newSizePanel.add(p1,BorderLayout.CENTER);
	
//	setNewSize(rayFrame.getContentPane().getSize().height,rayFrame.getContentPane().getSize().width);
	setNewSize(300, 300);
	
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
		    //	outputFormat.setSelectedIndex(0);
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
		    //outputFormat.setSelectedIndex(0);
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
	//DpiPanel.add(p3);
	p3.add(new JLabel("  DPI:"));
	dpi.addKeyListener(new KeyListener() {
		public void keyReleased(KeyEvent keyEvent) {
		    
		    updateLabel_newSize();
		    //outputFormat.setSelectedIndex(0);
		}
		
		public void keyPressed(KeyEvent keyEvent) {
		}
		
		public void keyTyped(KeyEvent keyEvent) {
		}
	    });
	//	p3.add(dpi);
	
	measure=new JComboBox();
	
	measure.insertItemAt("pixel",0);
	measure.insertItemAt("cm",1);
	measure.insertItemAt("inch",2);
	
	measure.setSelectedIndex(0);
	
	measure.addActionListener(new ActionListener(){
		public void actionPerformed(ActionEvent ae){
		    setHeight(Double.valueOf(height.getText()).doubleValue());
		    setWidth(Double.valueOf(width.getText()).doubleValue());
		    if(measure.getSelectedIndex()==0){
			setNewSize(Double.valueOf(height.getText()).intValue(),
				   Double.valueOf(width.getText()).intValue());
		    }
		    if(measure.getSelectedIndex()==1){
			setNewSize((int)((Double.valueOf(dpi.getText()).intValue())*(Double.valueOf(height.getText()).doubleValue())/2.4),
				   (int)((Double.valueOf(dpi.getText()).intValue())*(Double.valueOf(width.getText()).doubleValue())/2.4));
		    }
		    if(measure.getSelectedIndex()==2){
			setNewSize((int)((Double.valueOf(dpi.getText()).intValue())*(Double.valueOf(height.getText()).doubleValue())),(int)((Double.valueOf(dpi.getText()).intValue())*(Double.valueOf(width.getText()).doubleValue())));
		    }
		}
	    });
	
	
	//JPanel p35 = new JPanel(new GridLayout(1, 2));
	//panel.add(p35);
	//	p1.add(new JLabel("Unit:"));
	//	p1.add(measure);
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
	
	
	fpsPanel.add(new JLabel(" frames per sec.: "));
	fpsPanel.add(this.FramesPerSecond);
	
	lengthPanel.add(new JLabel(" seconds: "));
	lengthPanel.add(this.Time);
	
	Time.addKeyListener(new KeyListener() {
		public void keyReleased(KeyEvent keyEvent) {
		    
		    updateLabel_newSize();
		    //ROTATE_Y_AXIS_Button.setSelected(true);
		}
		
		public void keyPressed(KeyEvent keyEvent) {
		}
		
		public void keyTyped(KeyEvent keyEvent) {
		}
	    });
	
	
	standardDim=new JComboBox();
	standardDim.insertItemAt("choose a size (in pixels)",0);	
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
		          final JFrame frame2 = new JFrame("save movie " + projectName);

		          // adding container
		          Container contentPane = frame2.getContentPane();

		          if (surfex_.inAnApplet) {
		            ActionListener aL = new ActionListener() {
		              public void actionPerformed(ActionEvent actionEvent) {
		                String command2 = actionEvent
		                    .getActionCommand();
		                //          System.out.println("com:"+command2);
		                // check if doubleclickt or if "open" was pressed
		                if (command2.equals("create and save as")) {
		                  // creating complete filepath
		                  String filelocation = tf.getText();
		                  //              System.out.println("file:"+filelocation);
		                  // ausgabe fehler bein z.B.: d:\ -> d:\\ bzw \ -> \\
		                  //          System.out.println("filelocation : "+ filelocation + "\n");

		                  /*    project.saveMovie(filelocation, 3,
		                   ditheredCb.isSelected(),
		                   antialiasing.isSelected(),
		                   _3d.isSelected(),
		                   Integer.parseInt(height
		                   .getText()),
		                   Integer.parseInt(width
		                   .getText()),
		                   Integer.parseInt(dpi
		                   .getText()));
		                   */
		                  /*
		                   saveMovie(filelocation, 3,
		                   ditheredCb.isSelected(),
		                   antialiasing.isSelected(),
		                   _3d.isSelected(),
		                   Integer.parseInt(height
		                   .getText()),
		                   Integer.parseInt(width
		                   .getText()),
		                   Integer.parseInt(dpi
		                   .getText()));
		                   */
//		                  System.out.println("fps:"+((Integer)FramesPerSecond.getModel().getValue()).intValue());
				  surfex_.showMoviePreview = true;
		                  SaveMovie saveThread = new SaveMovie(
		                      filelocation, 
				      surfex_, 
		                      ((Integer)FramesPerSecond.getModel().getValue()).intValue(),
		                      Integer.parseInt(dpi.getText()),
		                      ((colorType.getSelectedIndex())==1),
		                      ((antialiasingQuality.getSelectedIndex())!=0), 
				      Integer.parseInt(Time.getText()),
				      omitType.getSelectedIndex(),
		                      Integer.parseInt(height.getText()),
		                      Integer.parseInt(width.getText()),
		                      (colorType.getSelectedIndex()==2), //RotType
		                      1, 
				      surfex_.getCurrentProject(),
		                      surfex_.getCurrentProject().jv4sx.getCamPos(), 
				      surfex_.getCurrentProject().jv4sx.getViewDir(), 
				      surfex_.getCurrentProject().jv4sx.getUpVector(), 
				      surfex_.getCurrentProject().jv4sx.getRightVector(),
				      ((Integer)XrotSpinner.getModel().getValue()).intValue(),
				      ((Integer)YrotSpinner.getModel().getValue()).intValue(),
				      ((Integer)ZrotSpinner.getModel().getValue()).intValue(),
				      getParametersToRun());
		                  saveThread.start();

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
		            if (colorType.getSelectedIndex()==1) {
		              tf = new JTextField("test.tif");
		            } else {
		              tf = new JTextField("test.jpg");
		            }
		            contentPane.add(tf, BorderLayout.CENTER);
		            JPanel tmpPanel = new JPanel();
		            tmpPanel.setLayout(new FlowLayout());
		            if (colorType.getSelectedIndex()==1) {
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
		          } else {
		            //          System.out.println("filechooser...");
		            // creating Filechooser
		            JFileChooser fileChooser = new JFileChooser(
		                surfex_.currentDirectory);
			    fileChooser.setDialogType(JFileChooser.SAVE_DIALOG);
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
		                if (command2.equals(JFileChooser.APPROVE_SELECTION)) {
		                  File selectedFile = theFileChooser
		                      .getSelectedFile();
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

		                  //                System.out.println("saveFile...");
		                  /*    project.saveFile(filelocation, 3,
		                   ditheredCb.isSelected(),
		                   antialiasing.isSelected(),
		                   Integer.parseInt(height
		                   .getText()),
		                   Integer.parseInt(width
		                   .getText()),
		                   Integer.parseInt(dpi
		                   .getText()));
		                   */
//		                  System.out.println("fps:"+((Integer)FramesPerSecond.getModel().getValue()).intValue());
		                  SaveMovie saveThread2 = new SaveMovie(
		                      filelocation, surfex_, //Integer
		                          //.parseInt((Integer)
		                      ((Integer)FramesPerSecond.getModel().getValue()).intValue(),//),
		                      Integer.parseInt(dpi.getText()),
		                      (colorType.getSelectedIndex()==1),
		                      (antialiasingQuality.getSelectedIndex()!=0), Integer
		                          .parseInt(Time.getText()),
				      omitType.getSelectedIndex(),
		                      Integer.parseInt(height.getText()),
		                      Integer.parseInt(width.getText()),
		                      (colorType.getSelectedIndex()==3), //RotType
		                      0, surfex_.getCurrentProject(),
		                      surfex_.getCurrentProject().jv4sx
		                          .getCamPos(), surfex_
		                          .getCurrentProject().jv4sx
		                          .getViewDir(), surfex_
		                          .getCurrentProject().jv4sx
		                          .getUpVector(), surfex_
		                          .getCurrentProject().jv4sx
		                          .getRightVector(),
		                          ((Integer)XrotSpinner.getModel().getValue()).intValue(),
		                          ((Integer)YrotSpinner.getModel().getValue()).intValue(),
		                          ((Integer)ZrotSpinner.getModel().getValue()).intValue()
		                          ,getParametersToRun());
		                  saveThread2.start();

		                  //              saveFile(filelocation);
		                } else {
		                  //if(command.equals(JFileChooser.CANCEL_SELECTION)){
		                  frame2.setVisible(false);
		                  //}
		                }
		              }
		             };

		          
		                FileFilter acfFilter = new ExtensionFileFilter(
		                    "all common files", new String[] {
		                        "gif", "mng"});
		                fileChooser.addChoosableFileFilter(acfFilter);
		                FileFilter spcFilter;
		                spcFilter = new ExtensionFileFilter("*.mng",
		                    new String[] { "mng" });
		                fileChooser.addChoosableFileFilter(spcFilter);
		                spcFilter = new ExtensionFileFilter("*.gif",
		                      new String[] { "gif" });
		                fileChooser.addChoosableFileFilter(spcFilter);
		                
		            

		            // adding actionListener
		            fileChooser.addActionListener(actionListener);
		            frame2.pack();
		          } // end of else (i.e. !inAnApplet)

		          frame2.setVisible(true);

		          //oeffen den Kram unter filename;

		          // end: command == "load"
		        }
		        if (command.hashCode() == 
			    DefaultButton.getText().hashCode()) {
			    //setVisible(false);
			    newSize.setText(rayFrame.getContentPane().getSize().width
					    + " x "
					    + rayFrame.getContentPane().getSize().height);
			    // rb[10].setSelected(true);
			    dpi.setText("72");
			    //FramesPerSecond.setText("15");
			    Time.setText("10");
			    height.setText(new String((new Integer(rayFrame
								   .getContentPane().getSize().height)).toString()));
			    width.setText(new String((new Integer(rayFrame
								  .getContentPane().getSize().width)).toString()));
			    colorType.setSelectedIndex(0);
			    antialiasingQuality.setSelectedIndex(0);//(true);
			    //_3d.setSelected(false);
			    aspectRatio.setSelected(true);
			    //ROTATE_Y_AXIS_Button.setSelected(true);
			    
			    
			    
		          
		          
		          /* 
		          for (i = 0; i <= 15; i++) {
		            last[i] = false;
		          }
		          last[0] = true;
		          last[11] = true;
		          last[14] = true;
*/
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
		setVisible(false);
	
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
				setNewSize(Double.valueOf(height.getText()).intValue(),Double.valueOf(width.getText()).intValue());
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
		}else{
			// angabe in cm / inch:
			// bis zu acht stellen (mit komma ) anzeigen:
			height.setText((h+"        ").substring(0,8).replaceAll(" ",""));
	
		}		
	}public void setWidth(double w){
		if(measure.getSelectedIndex()==0){
			//pixel ist selektiert, d.h. keine Nachkommastellen anzeigen!

			width.setText(((int)w)+"");
		}else{
			// angabe in cm / inch:
			// bieigen:
			width.setText((w+"        ").substring(0,8).replaceAll(" ",""));
	
		}
	}

	  
	  class removeListener implements ActionListener {
	        public void actionPerformed(ActionEvent e) {
	            //This method can be called only if
	            //there's a valid selection
	            //so go ahead and remove whatever's selected.
	            int index = parameterList.getSelectedIndex();
	            
//	          rausfinden, wo man den Parameter in ComboBox einfuegen kann:
	        int i=0;
	        //System.out.;
	        for(i=0;i<paramName.getItemCount();i++){
	          if(((String)paramName.getItemAt(i)).compareTo((String)parameterList.getSelectedValue())>=0){
	        //    i--;
	            break;
	          }
	        }
	        
	        paramName.insertItemAt(parameterList.getSelectedValue(),i);
	        
	            
	            
	            listModel.remove(index);

	            int size = listModel.getSize();

	            if (size == 0) { //Nobody's left, disable firing.
	                removeButton.setEnabled(false);

	            } else { //Select an index.
	                if (index == listModel.getSize()) {
	                    //removed item in last position
	                    index--;
	                }
	                
	                parameterList.setSelectedIndex(index);
	                parameterList.ensureIndexIsVisible(index);
	            }
	            

	            SwingUtilities.updateComponentTreeUI(paramName);

	            SwingUtilities.updateComponentTreeUI(listScrollPane);
	        }
	        
	        
	        
	    }

	    //This listener is shared by the text field and the hire button.
	    class addListener implements ActionListener, DocumentListener {
	        private boolean alreadyEnabled = false;
	        private JLabel button;

	        public addListener(JLabel button) {
	            this.button = button;
	        }

	        //Required by ActionListener.
	        public void actionPerformed(ActionEvent e) {
	            String name = (String) paramName.getSelectedItem();

	            if(paramName.getItemCount()!=0){
	         //   System.out.println("dabei --------------");
	              removeButton.setEnabled(true);
	            int index = parameterList.getSelectedIndex(); //get selected index
	            if (index == -1) { //no selection, so insert at beginning
	                index = 0;
	            } else {           //add after the selected item
	                index++;
	            }
	            
	            // In ScrollPane kopieren und aus ComboBox loeschen
	            int i;
	            String Item =(String)paramName.getSelectedItem();
	            for(i=0;i<parameterList.getModel().getSize();i++){
	          if(((String)parameterList.getModel().getElementAt(i)).compareTo(Item)>=0){
	        //    i--;
	            break;
	          }
	        }
	            
	            listModel.insertElementAt(Item, i);
	            //System.out.println();
	            paramName.removeItemAt(paramName.getSelectedIndex());
	            SwingUtilities.updateComponentTreeUI(paramName);
	            //If we just wanted to add to the end, we'd do this:
	            //listModel.addElement(employeeName.getText());

	            //Reset the text field.
	            paramName.requestFocusInWindow();
	           // paramName.setText("");

	            //Select the new item and make it visible.
	            parameterList.setSelectedIndex(index);
	            parameterList.ensureIndexIsVisible(index);
	            }
	        }
	        
//	      Required by DocumentListener.
	        public void insertUpdate(DocumentEvent e) {
	            enableButton();
	        }

	        //Required by DocumentListener.
	        public void removeUpdate(DocumentEvent e) {
	            handleEmptyTextField(e);
	        }

	        //Required by DocumentListener.
	        public void changedUpdate(DocumentEvent e) {
	            if (!handleEmptyTextField(e)) {
	                enableButton();
	            }
	        }
	        
	        private void enableButton() {
	            if (!alreadyEnabled) {
	                button.setEnabled(true);
	            }
	        }
	        private boolean handleEmptyTextField(DocumentEvent e) {
	            if (e.getDocument().getLength() <= 0) {
	                button.setEnabled(false);
	                alreadyEnabled = false;
	                return true;
	            }
	            return false;
	        }
	    }

	    //This method is required by ListSelectionListener.
	    public void valueChanged(ListSelectionEvent e) {
	        if (e.getValueIsAdjusting() == false) {

	            if (parameterList.getSelectedIndex() == -1) {
	            //No selection, disable fire button.
	                removeButton.setEnabled(false);

	            } else {
	            //Selection, enable the fire button.
	                removeButton.setEnabled(true);
	            }
	        }
	    }
	    

	    
	    private String[] getParametersToRun(){
	      String[] l=new String[parameterList.getModel().getSize()];
	      int i;
	      for(i=0;i<parameterList.getModel().getSize();i++){
	        l[i]=(String)parameterList.getModel().getElementAt(i);
	      }
	      
	      return l;
	    }
} // end class SavePicDialog

