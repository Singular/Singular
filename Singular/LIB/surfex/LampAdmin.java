////////////////////////////////////////////////////////////////////////
//
// This file LampAdmin.java is part of SURFEX.
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
import java.awt.FlowLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.BufferedReader;
import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ListIterator;
import java.util.*;

import javax.swing.*;
import javax.swing.SwingUtilities;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import jv.vecmath.*;
//////////////////////////////////////////////////////////////
//
// class ParameterAdmin
//
//////////////////////////////////////////////////////////////

public class LampAdmin extends JFrame implements ActionListener {

	final int N=7; // ANzahl Objekte pro Lampe in GUI 
	JPanel[] lampPanel=new JPanel[N+1];
	JPanel[] lampPanelContent=new JPanel[N+1];
	
	boolean lastRAB;

	//LampAdminUpdater lampAdminUpdater;
	
	//JComboBox paramComboBox; // used for SaveMovieDialog 
 
	public Vector lampList = new Vector();

//	updateJV4SX();
	double lastScale = 1.0;
	PdVector lastViewDir = new PdVector(3.2, 0.3, -0.9);
	PdVector lastPosition = new PdVector(10.0, 20.0, -20.0);
	PdVector lastUpVector = new PdVector(0.0, 0.0, 0.0);
	PdVector lastInterest = new PdVector(0.0, 0.0, 0.0);
	
	Project pro;

	Lamp theDarkLamp;
	
	int selectedLamp;
	
	boolean lampIsSelected=false;
	
	surfex surfex;

	RayFrame LampPrevRayFrame;//
	
	JButton addButton = new JButton();

	JSlider scaleSlider;

	public jv4surfex jv4sx = null;

	boolean showOnlySelectedLamp=true;
	
	JLabel scaleLabel;

	JPanel Panel = new JPanel(new BorderLayout());

	JButton bt7 = new JButton();

//	JCheckBox raytraceAlways = new JCheckBox("permanently", true);

	JCheckBox raytracePreview = new JCheckBox("ray preview", true);

	JCheckBox showTriangPrev = new JCheckBox("triang prev", true);

	JCheckBox antialiasing = new JCheckBox("antialiasing", false);

	JButton bgColorButton = null;

	String tmp_surfex_file = "tmp_surfex.jpg";

	UpdateRayframeImmediatlyThread updateRayframeImmediatlyThread;

	LampAdmin(surfex su, Project pro) {

		super("Configure lamps of " + pro.filename);
		surfex = su;

		makeDarkLamp();
		this.pro = pro;
		//		 create a new temporary filename: tmp_surfex_file
		Random myRNG = new Random();
		int myRN = myRNG.nextInt(99999999);
		//System.out.println("rn:"+myRN);
		tmp_surfex_file = "tmp_surfex_lampPrev" + myRN + "_"
				+ pro.ProjectNumber + ".jpg";


		if (surfex.iaa) {
			jv4sx = new jv4surfex(true, surfex.theApplet, surfex, 624,100);
			// surfex.theApplet = theApplet; 
		} else {
			jv4sx = new jv4surfex(false, null, surfex,624,100);//, 624);
		}
		//System.out.println(vec2Str(jv4sx.getCamPos()));
		
	
			
			//jv4sx.show();
			
			

		//paramComboBox=new JComboBox();

		JPanel controlPanel = new JPanel(new FlowLayout());
		//JPanel flowPanel = new JPanel(new FlowLayout());
		JButton saveas;
		try {
			if ((surfex.filePrefix).equals("")) {
				saveas = new JButton(new ImageIcon("saveas.gif"));
			} else {
				saveas = new JButton(new ImageIcon(new URL(surfex.filePrefix
						+ "saveas.gif")));
			}
		} catch (MalformedURLException e) {
			System.out.println(e.toString());
			saveas = new JButton();
		}
		saveas.setToolTipText("save this Lamp settings" + pro.projectName
				+ " as");
		saveas.addActionListener(this);
		saveas.setActionCommand("saveas");
		//if(!surfex_.inAnApplet) {
		controlPanel.add(saveas);

		getContentPane().setLayout(new BorderLayout());

		
		lampPanel[0]=new JPanel(new BorderLayout());
		for(int kkk=0;kkk<N;kkk++){
			lampPanel[kkk+1]=new JPanel(new BorderLayout());
			lampPanelContent[kkk]=new JPanel(new GridLayout(11,1));
			lampPanel[kkk].add(lampPanelContent[kkk],BorderLayout.WEST);
			lampPanel[kkk].add(lampPanel[kkk+1],BorderLayout.CENTER);
		}
		//lampPanel[N]=new JPanel(new BorderLayout());
		//lampPanel[N]=new JPanel(new BorderLayout());
		
		
		lampPanelContent[N]=new JPanel(new GridLayout(11,1));
		
		lampPanel[N].add(lampPanelContent[N],BorderLayout.WEST);
		lampPanelContent[0].add(new JLabel(" Lamp "));
		lampPanelContent[1].add(new JLabel(" preview "));
		lampPanelContent[2].add(new JLabel(" x-Koord "));
		lampPanelContent[3].add(new JLabel(" y-Koord "));
		lampPanelContent[4].add(new JLabel(" z-Koord "));
		lampPanelContent[5].add(new JLabel(" intensity "));
		lampPanelContent[6].add(new JLabel(" color "));
		lampPanelContent[7].add(new JLabel(" shines "));
		
		
		//lampPanel[N].add(lampPanel[N-1],BorderLayout.CENTER);
		
		
		
//		add(lampPanelGes, BorderLayout.CENTER);

		
		
//  ADD-BUTTON
/*		addButton.setText("add lamp");
		addButton.setToolTipText("add lamp");
		//    getContentPane().add(bt1);
		addButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				addButtonActionPerformed(evt);
			}
		});
		
		controlPanel.add(addButton);
*/
		//		 a button for choosing the background color:
		bgColorButton = new JButton();
		bgColorButton.setBackground(Color.white);
		bgColorButton.setToolTipText("select the background color");
		bgColorButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				bgColorButton.setBackground(JColorChooser.showDialog(null,
						"change the background color", bgColorButton
								.getBackground()));
			}
		});
		//controlPanel.add(new JLabel("bg-color:"));
	//	controlPanel.add(bgColorButton);

	/*	raytraceAlways.addChangeListener(new ChangeListener() {
			public void stateChanged(ChangeEvent evt) {
				raytraceAlwaysChanged(evt);
			}
		});
		controlPanel.add(raytraceAlways);
*/
		raytracePreview.addChangeListener(new ChangeListener() {
			public void stateChanged(ChangeEvent evt) {

				//if(ChangeEvent.)
				//System.out.println("1 prev invisible");
				raytraceAlwaysChanged(evt);
				
			}
		});
		controlPanel.add(raytracePreview);

		showTriangPrev.addChangeListener(new ChangeListener() {
			public void stateChanged(ChangeEvent evt) {
				showTriangPrevChanged(evt);
			}
		});
		controlPanel.add(showTriangPrev);

		JScrollPane lampPane = new JScrollPane(lampPanel[0]);

		Panel.add(controlPanel, BorderLayout.NORTH);
		Panel.add(lampPane, BorderLayout.CENTER);



		    this.addWindowListener(new WindowAdapter() {
		      public void windowClosing(WindowEvent evt) {
		       System.out.println("closeevent");
		      	lampManagerInvisible();
		      }
		    });

			
		    int tret;
		

		    loadDefaultLampSettig1();
		
		lampPanelContent[0].add(new JLabel(" ")); // looks nicer
		
		

		this.setContentPane(Panel);
		Panel.setBorder(BorderFactory.createEmptyBorder(20, 20, 20, 20));
		this.pack();
		this.setVisible(false);
		
		lastRAB=this.raytracePreview.isSelected();
		//showall();

		LampPrevRayFrame = new RayFrame("raytraced lamp preview");
		LampPrevRayFrame.setSize(200 + LampPrevRayFrame.getInsets().left
				+ LampPrevRayFrame.getInsets().right, 200
				+ LampPrevRayFrame.getInsets().top
				+ LampPrevRayFrame.getInsets().bottom);
		LampPrevRayFrame.setLocation(624, 200);
		LampPrevRayFrame.setInfo("your viewing direction\n is the lamp's one");

		jv4sx.setPosTextFields2(getSelectedLamp().xPos,getSelectedLamp().yPos,getSelectedLamp().zPos);

		updateJV4SXandReconstructLastView();
		//		System.out.println(vec2Str(jv4sx.getCamPos()));
				
		//		jv4sx.showDefault();
			
		
		updateRayframeImmediatlyThread = new UpdateRayframeImmediatlyThread(
				LampPrevRayFrame, surfex, pro, jv4sx,pro.jv4sx, getModifiedLampList(), 
				showOnlySelectedLamp,getSelectedLamp());
		updateRayframeImmediatlyThread.start();

		
		
		//
	
		SwingUtilities.updateComponentTreeUI(this);
	}
	
	public String vec2Str(double[] v) {
		return ("<" + v[0] + "," + v[1] + "," + v[2] + ">");
	}
	
	public Lamp getSelectedLamp(){
		//System.out.println("sdgdfgfdgfdg");
		return ((Lamp)lampList.elementAt(selectedLamp));
	}

	public Lamp getCameraLamp(){
		 // Lampe die von der Kamera aus strahlt
		// mit helligkeit und farbe der SelectedLamp
		// die man gerade einstellen will
		return getSelectedLamp().previewLamp(0.0,0.0,100.0);
	}

	public Vector getModifiedLampList(){
		Vector lamps=new Vector();
		lamps.add(getCameraLamp());
		
		// switch off other lamps
		makeDarkLamp();
		lamps.add(theDarkLamp);
		lamps.add(theDarkLamp);
		lamps.add(theDarkLamp);
		lamps.add(theDarkLamp);
		lamps.add(theDarkLamp);
		lamps.add(theDarkLamp);
		lamps.add(theDarkLamp);
		lamps.add(theDarkLamp);
		return lamps;
		
	}
	
	public void loadDefaultLampSettig1(){
		
		newLamp(-100.000000,100.000000,100.000000,255,255,255,50,true);
		newLamp(0.000000,100.000000,100.000000,255,255,255,50,false);
		newLamp(100.000000,100.000000,100.000000,255,255,255,70,true);
		newLamp(-100.000000,0.000000,100.000000,255,255,255,50,false);
		newLamp(0.000000,0.000000,100.000000,255,255,255,50,false);
		newLamp(100.000000,0.000000,100.000000,255,255,255,50,false);
		newLamp(-100.000000,-100.000000,100.000000,255,255,255,50,false);
		newLamp(0.000000,-100.000000,100.000000,255,255,255,30,true);
		newLamp(100.000000,-100.000000,100.000000,255,255,255,50,false);
		
		
		 lampIsSelected=true;
		setSelectedLamp(2); // Lampe 3 als selected betrachten
	//	 lampIsSelected=true
		//	System.out.println("t1"+getSelectedLamp().getXpos());
	}
	
	public void makeDarkLamp(){
		Color c=new Color(0,0,0);
		theDarkLamp=getNewLamp(0.0,0.0,0.0,c,0,true);
	}
	
	public Lamp getNewLamp(double x, double y, double z,
			Color c, 
			int i,
			boolean shines){
		
		Lamp lamp=new Lamp(pro,this);//(Lamp)lampList.lastElement();
		lamp.setKoords(x,y,z);
		lamp.setIntensity( i);
		lamp.setColor(c);
		lamp.setIsShining(shines);
		//SwingUtilities.updateComponentTreeUI(lamp);
		return lamp;
		
	}
	
	public Vector getLampsForJv4sx(){
		// returned alle Lampen die angeschaltet sind.
		
		Vector list=new Vector();
		ListIterator li=lampList.listIterator();
		Lamp tmp;
		while(li.hasNext()){
			tmp=((Lamp)li.next());
			if((tmp.isShining()) && (tmp.lampNo-1 != selectedLamp)){
				list.add(tmp);
//				System.out.println(tmp.lampNo+" "+selectedLamp);
			}
		}		
//		while(list.size()<9){
	//		list.add(theDarkLamp);
		//	System.out.println(theDarkLamp.getXpos());
			
	//	}
		return list;
	}
	
	public boolean lampIsSelected(){
		return lampIsSelected;//selectedLamp)==null)?false:true;
	}
	public synchronized void setSelectedLamp(int no){
		
		if(lampIsSelected()){
			if(updateRayframeImmediatlyThread!=null){
				updateRayframeImmediatlyThread.stop();
			}
		getSelectedLamp().setSelected(false);
		selectedLamp=no;
		if(jv4sx!=null){
			jv4sx.setPosTextFields2(
			    getSelectedLamp().xPos,getSelectedLamp().yPos,getSelectedLamp().zPos);
		//System.out.println("joahr----"+((Lamp)getAllLamps().firstElement()).getXpos());
			jv4sx.setLamps(getLampsForJv4sx());
		//	System.out.println("joahr2");
		
		}
		lampIsSelected=true;
		}else{
			selectedLamp=no;
		}
		
		
		getSelectedLamp().setSelected(true);
	//	System.out.println("7"+vec2Str(jv4sx.getCamPos()));
	//	System.out.println("t1"+getSelectedLamp().getXpos());
		
		updatePreview();
	//	System.out.println("8"+vec2Str(jv4sx.getCamPos()));
	//	System.out.println("t1"+getSelectedLamp().getXpos());
	//	System.out.println("joahr3");
	}
	
	public void updatePreview(){
		jv4sx.showDefault();
		jv4sx.disp.getCamera().setPosition(getSelectedLamp().getPos());
		
		
		newUpdateRayframeImmediatlyThread();
		jv4sx.updateDisp();
	}
	
	public Vector getAllLamps(){
		// returned alle Lampen die angeschaltet sind.
		
		Vector list=new Vector();
		ListIterator li=lampList.listIterator();
		Lamp tmp;
		while(li.hasNext()){
			tmp=((Lamp)li.next());
			if(tmp.isShining()){
				list.add(tmp);
			}
		}		
		while(list.size()<9){
			list.add(theDarkLamp);
		//	System.out.println(theDarkLamp.getXpos());
			
		}
		return list;
	}
	
	public void newLamp(double x, double y, double z,
			int r, int g, int b, 
			int i,
			boolean shines){
		newLamp();
		Lamp lamp=(Lamp)lampList.lastElement();
		lamp.setKoords(x,y,z);
		lamp.setIntensity( i);
		lamp.setColor(r,g,b);
		lamp.setIsShining(shines);
		//SwingUtilities.updateComponentTreeUI(lamp);
		
		
	}
	
	

	public void raytracePreviewChanged(ChangeEvent evt) {

	}

	public void showTriangPrevChanged(ChangeEvent evt) {
//		System.out.println( raytracePreviewisValid() +" "+ lastRAB);
		if (showTriangPrev.isSelected() != lastRAB) {

//			System.out.println("34 prev invisible");
			if (showTriangPrev.isSelected()) {
	//			System.out.println("22prev invisible");
				//startRaytraceAlways();
				jv4sx.setVisible(true);
				lastRAB = showTriangPrev.isSelected();
			}else {
		//		System.out.println("prev invisible");
				jv4sx.setVisible(false);
 
				lastRAB = showTriangPrev.isSelected();
			}
		}
	}

	public synchronized void raytraceAlwaysChanged(ChangeEvent evt) {

		//System.out.println( raytracePreviewisValid() +" "+ lastRAB);
		if (raytracePreview.isSelected() != lastRAB) {

//			System.out.println("34 prev invisible");
			if (raytracePreview.isSelected()) {
	//			System.out.println("22prev invisible");
				startRaytraceAlways();
				this.LampPrevRayFrame.setVisible(true);
				lastRAB = raytracePreview.isSelected();
			}else {
		//		System.out.println("prev invisible");
				this.LampPrevRayFrame.setVisible(false);
 
				lastRAB = raytracePreview.isSelected();
			}
		}
	}

	public void startRaytraceAlways() {
		//	surfex_.configFrame.last[4] = true;
		//	double ang[] = surfex_.getCurrentProject().eqAdm.getAngles();
		//	double scale[] = surfex_.getCurrentProject().eqAdm.getScales();
		newUpdateRayframeImmediatlyThread();
	//	updateRayframeImmediatlyThread.start();

	}

	public void newUpdateRayframeImmediatlyThread() {
	/*	if(updateRayframeImmediatlyThread!=null){
		updateRayframeImmediatlyThread.stop();
		}*/
	//	updateRayframeImmediatlyThread = new UpdateRayframeImmediatlyThread(
	//			this.LampPrevRayFrame, surfex, pro, jv4sx, this.getModifiedLampList(),showOnlySelectedLamp,getSelectedLamp());
	}

	public void addButtonActionPerformed(ActionEvent evt) {
		newLamp();
	}

	public void actionPerformed(ActionEvent e) {
		// jede Action die Performed wird kann nur ein close sein ...
		// --> close all connected windows
		System.out.println("close");
		lampManagerInvisible();
	}
	
	
	
	public void lampManagerInvisible(){
		setVisible(false);
		this.LampPrevRayFrame.setVisible(false);
		jv4sx.setVisible(false);
	}

	public void newLamp() {
		//  System.out.println("newParameter");
		Lamp lamp = new Lamp(pro, this,lampPanelContent);

		// find a non-used parameter number:
		int n = lampList.size() + 1;
		ListIterator li = lampList.listIterator();
		while (li.hasNext()) {
			if (n == ((Lamp) (li.next())).lampNo) {
				// if the parameter number n is already in use
				n = n + 1; // try the next number
				// reset the list iterator to the first element in the list:
				while (li.hasPrevious()) {
					li.previous();
				}
			}
		}
		lamp.setLampNo(n);
		//  System.out.println("parNo:"+n);

		// add this parameter to the list:
		lampList.add(lamp);
		//  System.out.println("lampList:"+lampList.size());
		//lampPanel.add(lamp);
/*
		if (this.surfex.getCurrentProject()!=null){
		if (this.surfex.getCurrentProject().saveMovieDialog != null) {
			// update only if exists...
			SwingUtilities.updateComponentTreeUI(this.surfex
					.getCurrentProject().saveMovieDialog);
		}
		}*/
		//SwingUtilities.updateComponentTreeUI(this);
		//SwingUtilities.updateComponentTreeUI(this);
	}

	public String save() {
		String str = "";
		ListIterator li = lampList.listIterator();
		str += "////////////////// LAMPS: /////////////////////////" + "\n";
		str += lampList.size() + "\n";
		while (li.hasNext()) {
			str += ((Lamp) li.next()).saveYourself();
		}
		return (str);
	}

	public void loadParameters(BufferedReader bs, String datatype) {
		int k;
		try {
			String nix = bs.readLine();
			String zeile1;
			boolean first = true;

			for (k = Integer.parseInt(bs.readLine()); k > 0; k--) {
				zeile1 = bs.readLine();//) != null) {
				newLamp();
				if (datatype.equals("specify")) {
					String strline;
					try {
						bs.mark(255);
						strline = bs.readLine();
						while ((strline.equals("parno:"))
								|| (strline.equals("fromtoval:"))) {
							//          System.out.println("strline:"+strline);
							if (strline.equals("parno:")) {
								((OneParameter) lampList.lastElement())
										.setParNo(Integer.parseInt(bs
												.readLine()));
							}
							if (strline.equals("fromtoval:")) {
								((OneParameter) lampList.lastElement()).oldFrom = Double
										.parseDouble(bs.readLine());
								((OneParameter) lampList.lastElement()).newFrom = ((OneParameter) lampList
										.lastElement()).oldFrom;

								((OneParameter) lampList.lastElement()).oldTo = Double
										.parseDouble(bs.readLine());
								((OneParameter) lampList.lastElement()).newTo = ((OneParameter) lampList
										.lastElement()).oldTo;

								((OneParameter) lampList.lastElement()).parSlider
										.setValue(Integer.parseInt(bs
												.readLine()));
								((OneParameter) lampList.lastElement())
										.updateparSliderAndLabel();
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
					((OneParameter) lampList.lastElement()).setParNo(Integer
							.parseInt(bs.readLine()));
					((OneParameter) lampList.lastElement()).oldFrom = Double
							.parseDouble(bs.readLine());
					((OneParameter) lampList.lastElement()).newFrom = ((OneParameter) lampList
							.lastElement()).oldFrom;
					((OneParameter) lampList.lastElement()).oldTo = Double
							.parseDouble(bs.readLine());
					((OneParameter) lampList.lastElement()).newTo = ((OneParameter) lampList
							.lastElement()).oldTo;
					((OneParameter) lampList.lastElement()).parSlider
							.setValue(Integer.parseInt(bs.readLine()));
					((OneParameter) lampList.lastElement()).from.setText(""
							+ ((OneParameter) lampList.lastElement()).newFrom);
					((OneParameter) lampList.lastElement()).to.setText(""
							+ ((OneParameter) lampList.lastElement()).newTo);
					((OneParameter) lampList.lastElement())
							.updateparSliderAndLabel();
					((OneParameter) lampList.lastElement()).repaint();
				}
				if (datatype.equals("complete") || datatype.equals("onlystyle")) {
					//        ((OneParameter) lampList.lastElement()).cbox.setSelected((Boolean.valueOf(bs.readLine())).booleanValue());
				}
			}
		} catch (IOException e) {
			System.out.println(e);
		}
	}

	/*public Vector getParameterNames(){
	 Vector l = new Vector();//new String[lampList.size()];
	 
	 ListIterator li = lampList.listIterator();
	 while (li.hasNext()) {
	 l.add(((OneParameter)li.next()).nameLabel.getText()) ;
	 //i++;
	 }		
	 return l;
	 }*/

	public OneParameter getParameter(String s) {
		ListIterator li = lampList.listIterator();
		OneParameter tmpParameter;
		while (li.hasNext()) {
			tmpParameter = ((OneParameter) li.next());
			if (tmpParameter.nameLabel.getText().compareTo(s) == 0) {
				return tmpParameter;
			}

			//i++;
		}
		return null;
	}

	public void showall() {
		jv4sx.show();
		LampPrevRayFrame.setVisible(true);
		LampPrevRayFrame.toFront();
		setVisible(true);
		toFront();
	}
	
	public void lampManagerToFront(){
		this.setVisible(true);
		LampPrevRayFrame.setVisible(true);
		jv4sx.setVisible(true);
	}
	
	public void updateJV4SXandReconstructLastView() {
		//  System.out.println("updateJV4SXandRec...");
		//  jv4sx.disp.getCamera().setUpVector(lastUpVector);
		// jv4sx.disp.getCamera().setViewDir(lastViewDir);
		//jv4sx.disp.getCamera().setFullPosition(lastInterest, lastPosition, lastUpVector);

		jv4sx.changeFrameTitle("Lamppreview "+pro.projectName
				+ "(uses JavaView)");
		jv4sx.updateDisp();
	}
	
	

} // end class ParameterAdmin

