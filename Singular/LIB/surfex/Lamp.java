////////////////////////////////////////////////////////////////////////
//
// This file Lamp.java is part of SURFEX.
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
import java.awt.*;//Color;
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.PrintWriter;
import java.text.NumberFormat;

import javax.swing.*;//JButton;
import javax.swing.JCheckBox;
import javax.swing.JColorChooser;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.JTextField;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import java.util.*;

import jv.vecmath.*;

//////////////////////////////////////////////////////////////
//
// class Lamp
//
//////////////////////////////////////////////////////////////

public class Lamp extends JPanel {

	// Anfang Variablen

//	JButton deleteLampButton = new JButton("del");

	JButton colorButton;


	
	String lastX = "0";

	String lastY = "0";

	String lastZ = "0";

	String lastXj = "0";

	String lastYj = "0";

	String lastZj = "0";

	//Lichtstaerke lichtstaerke;

	//////////////////////////////
	Project project;

	public int lampNo = 0;

	public JCheckBox cbox = new JCheckBox("", true);
	
	public int oldFrom = 0;

	public int oldTo = 1;

	public int newFrom = 0;

	public int newTo = 1;

	

//	 zum rechnen
//	pcalc polyCalc = new pcalc();
	
	public JLabel intenseLabel = new JLabel("50");

	
	public JTextField from = new JTextField("0");

	public JTextField to = new JTextField("100");

	JLabel toLabel = new JLabel("to:");

	JLabel fromLabel = new JLabel("from:");

	JSlider intenseSlider = new JSlider(0, 100);//intenseSlider = null;


	// GUI

	public JLabel nameLabel = null;

	public JTextField xPos = new JTextField("0.000");

	public JTextField yPos = new JTextField("1.000");

	public JTextField zPos = new JTextField("0.000");

	//public JLabel parLabel = new JLabel("0.50000");

	JLabel xLabel = new JLabel(" x: ");

	JLabel zLabel = new JLabel(" z: ");

	JLabel yLabel = new JLabel(" y: ");

	JLabel PreviewPic;

	Lamp previewLamp;
	
	JButton setPos = new JButton("set Pos");;

	JButton intensity = new JButton("Intensity");

	LampAdmin lampAdmin;

	// Ende Variablen

	// Konstruktor
	Lamp(Project pro, LampAdmin lampAdmin,JPanel[] lampPanel) {
		project = pro;
		this.lampAdmin = lampAdmin;

		nameLabel = new JLabel("  " + lampNo+" ");
		lampPanel[0].add(nameLabel);

//		polyCalc.doPrint = false;
//		lichtstaerke = new Lichtstaerke(" intensity of " + nameLabel.getText()+" ");
		//panel.add(KoordsPanel);
		//panel.add(flowPanel);
		//  flowPanel.add(cbox);

		PreviewPic = new JLabel();
		lampPanel[1].add(setPos);
	//	lampPanel[2].add(xLabel);
		
		//xPos.setBounds(200, 40, 250, 100);//.setSize(200,50);
		lampPanel[2].add(xPos);
//		lampPanel[4].add(yLabel);
		lampPanel[3].add(yPos);

	//	lampPanel[6].add(zLabel);
		lampPanel[4].add(zPos);
		setPos.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ae) {

				updateLampAdmin();
				// PreviewFenser anpassen

				//lichtstaerke.setVisible(true);
			}
		});

	//	lampPanel[8].add(setPos);

		//lampPanel[9].add(deleteLampButton);
	//	deleteLampButton.setEnabled(false);

		//flowPanel.add(new JLabel(" "));

	/*	intensity.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ae) {
				lichtstaerke.setVisible(true);
			}
		});

		*/
		
	    
	    
	    
	    
		intenseSlider.setValue(50);
		intenseSlider.setMinorTickSpacing(1);
		intenseSlider.setMajorTickSpacing(10);
		intenseSlider.setPaintTicks(true);
		intenseSlider.addChangeListener(new ChangeListener() {
			public void stateChanged(ChangeEvent evt) {
				updateintenseSliderLabel();
			}
		});

		lampPanel[5].add(intenseSlider);
		
		colorButton = new JButton();
//		colorButton.setBorderPainted(true);
  //      colorButton.setBorder(BorderFactory.createBevelBorder(5,new Color(100,100,100),new Color(200,200,200)));//.createEtchedBorder());

		//colorButton.setText("");
		colorButton.setBackground(new Color(0, 180, 240));
		colorButton.setToolTipText("Select the \"light\" color");
		colorButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				setColor(JColorChooser.showDialog(null,
						"Change light color", colorButton.getBackground()));
			}
		});

	//	lampPanel[10].add(new JLabel(" color: "));
		lampPanel[6].add(colorButton);

//		lampPanel[2].add(new JLabel(" shines: "));
		lampPanel[7].add(cbox);

		
	}
	
	public synchronized void updateLampAdmin(){
		lampAdmin.setSelectedLamp(lampNo-1);
	}
	
	// Konstruktor ohne GUI:
	
	Lamp(Project pro, LampAdmin lampAdmin) {
		project = pro;
		this.lampAdmin = lampAdmin;
//		polyCalc.doPrint = false;
		intenseSlider.setValue(50);
		colorButton = new JButton();
		colorButton.setBackground(new Color(0, 180, 240));
	}
	
	public Lamp previewLamp(double x, double y, double z){
		previewLamp=getNewLamp(x,y,z,colorButton.getBackground(),getVol(),isShining());
		
		
		return previewLamp;
	}
	
	public Lamp getNewLamp(double x, double y, double z,
			Color c, 
			int i,
			boolean shines){
		
		Lamp lamp=new Lamp(project,lampAdmin);//(Lamp)lampList.lastElement();
		lamp.setKoords(x,y,z);
		lamp.setIntensity( i);
		lamp.setColor(c);
		lamp.setIsShining(shines);
		//SwingUtilities.updateComponentTreeUI(lamp);
		return lamp;
		
	}
	
	public void setColor(Color c){
		colorButton.setBackground(c);
		if(previewLamp!=null){
			previewLamp.setColor(c);
		}
	}
	
	public int getIntensity(){
		
		return intenseSlider.getValue();
	}
	
	public void setIntensity(int i){
		intenseSlider.setValue(i);
		updateintenseSliderLabel();
		if(previewLamp!=null){
			previewLamp.setIntensity(i);
		}
	}
	
	public synchronized void updateintenseSliderLabel()  {
		intenseLabel.setText(""+intenseSlider.getValue());	
		//SwingUtilities.updateComponentTreeUI(this);
	}

	public void setKoords(double x, double y, double z) {
		//grosses Pronlem :: NumberFormat arbeitet buggy.
		// d.h. er gibt manchmal nen leeren String zurueck,
		// was ihn spaeter z.b. in getXpos() abschiesst

		// bloss nicht die previewLamp updaten ;-)
		
		NumberFormat nf = NumberFormat.getNumberInstance(Locale.US);
		nf.setMaximumFractionDigits(3);//.setsetMinimumIntegerDigits(5); //
									   // ggf. mit fuehrenden Nullen ausgeben
		nf.setMinimumFractionDigits(3);//.setsetMinimumIntegerDigits(5); //
									   // ggf. mit fuehrenden Nullen ausgeben

		String newx = nf.format(x);
		int i = 0;
		while (newx.compareTo("") != 0 && ++i < 10) {
			newx = nf.format(x);
 		}
		xPos.setText(newx);
		i = 0;

		String newy = nf.format(y);
		while (newy.compareTo("") != 0 && ++i < 10) {
			newy = nf.format(y);
		}
		yPos.setText(newy);

		i = 0;
		String newz = nf.format(z);
		while (newz.compareTo("") != 0 && ++i < 10) {
			newz = nf.format(z);
		}
		zPos.setText(newz);
//		System.out.println("new coords:"+xPos.getText()+","+yPos.getText()+","+zPos.getText()+".");
	}



	public void setColor(int r, int g, int b) {
		colorButton.setBackground(new Color(r, g, b));
		if(previewLamp!=null){
			previewLamp.setColor(r,g,b);
		}
	}

	public boolean isSelected() {
		return (cbox.isSelected());
	}

	public void setIsShining(boolean b) {
		cbox.setSelected(b);
		//if(previewLamp!=nul)
		if(previewLamp!=null){
			previewLamp.setIsShining(b);
		}
	}

/*	public synchronized void updateKoords_InvokedByUpdateFrameImmediatlyThread_LampAdmin(
			jv4surfex jv4sx) {
		
		// bloss nicht die previewLamp updaten ;-)
		
		
		double[] ang = new double[3];
		ang[0] = jv4sx.getCamPos()[0];//-jv4sx.getCamPos()[0];
		ang[1] = jv4sx.getCamPos()[1];//.getCameraRotationYXZ()[1];//-jv4sx.getCamPos()[1];
		ang[2] = jv4sx.getCamPos()[2];//getCameraRotationYXZ()[2];//)-jv4sx.getCamPos()[2];
	//	System.out.println("r" + vec2Str(jv4sx.getCamPos()));
		
		if (!((lastX.compareTo(getXpos_str()) == 0)
				&& (lastY.compareTo(getYpos_str()) == 0) && (lastZ
				.compareTo(getZpos_str()) == 0))) {
 
			// der Benutzer hat die Koordinaten schriftlich geaendert
			// -> jv4sx anpassen!!
			try{
			PdVector v = getPos();
			v.multScalar(1.0);
			jv4sx.disp.getCamera().setPosition(v);
			jv4sx.setParameterWarning(false);
			//jv4sx.updateScaleSliderValue();
			
		//	System.out.println(10/getVecLength()+" - "+jv4sx.disp.getCamera().getScale());
			
			
			jv4sx.updateDisp();
			
			
			
			}catch(Exception e){
				//er hat wahrschinlich nen parameter im Textfeld stehehn und kann deswegen nicht nach double konvertieren
				jv4sx.setParameterWarning(true);
				
				
			}
			

			//				 geaenderte Daten speichern
			// damit man merkt, falls er sie wieder aendert
			//lastXj=Double.valueOf(ang[0]).toString();
			//lastYj=Double.valueOf(ang[1]).toString();
			//lastZj=Double.valueOf(ang[2]).toString();
			lastX = getXpos_str();
			lastY = getYpos_str();
			lastZ = getZpos_str();

		}
		if (!((lastXj.compareTo(Double.valueOf(ang[0]).toString()) == 0)
				&& (lastYj.compareTo(Double.valueOf(ang[1]).toString()) == 0) && (lastZj
				.compareTo(Double.valueOf(ang[2]).toString()) == 0))) {
			//			 d.h der Benutzer hat die Koordinaten nicht schriftlich geaendert
			// aber moeglicherweise im jv4sx-previefenster:
			//			
			// d.h. Benutzer hat jvview gedreht
			setKoords(ang[0], ang[1], ang[2]);
			//				 geaenderte Daten speichern
			// dami man merkt, falls er sie wieder aendert
			lastXj = Double.valueOf(ang[0]).toString();
			lastYj = Double.valueOf(ang[1]).toString();
			lastZj = Double.valueOf(ang[2]).toString();
			lastX = getXpos_str();
			lastY = getYpos_str();
			lastZ = getZpos_str();
		}
		

	}*/

	public String vec2Str(double[] v) {
		return ("<" + v[0] + "," + v[1] + "," + v[2] + ">");
	}

	public PdVector getPos() {
		return new PdVector(getXpos(), getYpos(), getZpos());
	}
	
	public double getVecLength(){
		
	//	System.out.println(Math.sqrt(2));
		return Math.sqrt(Math.pow(this.getXpos(),2)+Math.pow(this.getYpos(),2)+Math.pow(this.getZpos(),2));
	}

	public boolean isShining() {
		//System.out.println("sel");
		return cbox.isSelected();
	}

	public void setSelected(boolean b) {
		if(previewLamp!=null){
			previewLamp.setSelected(b);
		}
		if (b) {
			setPos.setBackground(new Color(0, 255, 0));
		} else {
		//	System.out.println("set false -----------------------------------------");
			setPos.setBackground(null);//new Color(100, 100, 100));
		}
		setPos.repaint();
	}
	
	

	public synchronized String getXpos_str() {
		String str = "";
		while (str.compareTo("") == 0) {
			str = xPos.getText();
			// Achtung, liefert manchmal den leeren String statt den richtigen
		}
//		System.out.println("xPos:"+str+".");
		return str;
	}

	public synchronized String getYpos_str() {
		String str = "";
		while (str.compareTo("") == 0) {
			str = yPos.getText();
			// Achtung, liefert manchmal den leeren String statt den richtigen
		}
		return str;
	}

	public synchronized String getZpos_str() {
		String str = "";
		while (str.compareTo("") == 0) {
			str = zPos.getText();
			// Achtung, liefert manchmal den leeren String statt den richtigen
		}
		return str;
	}

	public synchronized double getXpos() {
		String str = "";
		while (str.compareTo("") == 0) {
			str = xPos.getText();
			// Achtung, liefert manchmal den leeren String statt den richtigen
		}
//		System.out.println("xPos:"+str+".");
		return Double.valueOf(str).doubleValue();
	}

	public synchronized double getYpos() {
		String str = "";
		while (str.compareTo("") == 0) {
			str = yPos.getText();
			// Achtung, liefert manchmal den leeren String statt den richtigen
		}
		return Double.valueOf(str).doubleValue();
	}

	public synchronized double getZpos() {
		String str = "";
		while (str.compareTo("") == 0) {
			str = zPos.getText();
			// Achtung, liefert manchmal den leeren String statt den richtigen
		}
		return Double.valueOf(str).doubleValue();
	}

	public int getVol() {
		return getIntensity();
	}

	public int getRed() {
		return colorButton.getBackground().getRed();
	}

	public int getGreen() {
		return colorButton.getBackground().getGreen();
	}

	public int getBlue() {
		return colorButton.getBackground().getBlue();
	}

	public void setLampNo(int no) {
		if(previewLamp!=null){
			previewLamp.setLampNo(no);
		}
		lampNo = no;
		nameLabel.setText("   " + lampNo);
		//lichtstaerke.setTitle("l" + lampNo);
		//SwingUtilities.updateComponentTreeUI(lichtstaerke);
	}

	public void updateActionCommands(int internalCunr) {
	}

	public String getSurfCode(int lampNo, String[] runningParams,
		      double[] runningParamsValue) {
	    String str="";
	    str+="light"+lampNo+"_x ="+evalWithPar(getXpos_str(),runningParams,runningParamsValue)+";\n";
	    str+="light"+lampNo+"_y ="+evalWithPar(getYpos_str(),runningParams,runningParamsValue)+";\n";
	    str+="light"+lampNo+"_z ="+evalWithPar(getZpos_str(),runningParams,runningParamsValue)+";\n";
	    str+="light"+lampNo+"_vol ="+getVol()+";\n";
	    str+="light"+lampNo+"_red ="+getRed()+";\n";
	    str+="light"+lampNo+"_green ="+getGreen()+";\n";
	    str+="light"+lampNo+"_blue ="+getBlue()+";\n";
	    return str;
	}
	
	public double evalWithPar(String s,String[] runningParams,
		      double[] runningParamsValue){
		int i=0;
	//	System.out.println(s);
		if(runningParams!=null){
		for(int j=0;j<runningParams.length;j++){
//			System.out.println("rv:"+runningParamsValue[j]);
			s=s.replaceAll(runningParams[j],(new Double(runningParamsValue[j])).toString());
			
		}
		}
		
		s=s.replaceAll(" ","");
		
		s=s.replaceAll("\\*"," ");		
		/*
		System.out.println("s1:"+s);
		
		s= polyCalc.showAsPovrayCode(s);
		System.out.println("s2:"+s);

		if(s.length()>4){
		s=s.substring(4,s.length()-1);
		}	//Double.valueOf(s).doubleValue()*genauigkeit;
		
		// damit es waehrend der Eingabe keine Probleme gibt:
		s=s.replaceAll("\\+","");
		if(s.charAt(0)=='-'){
			s=s.replaceAll("-","");
			s="-"+s;
		}else{
			s=s.replaceAll("-","");
		}
		s=s.replaceAll("\\*","");
		s=s.replaceAll("/","");
		s=s.replaceAll("^","");
		System.out.println("s3:"+s);
		*/		
		
		try{
			return Double.valueOf(s).doubleValue();
		}catch(Exception e){
//			System.out.println("ERROR: erst parameter auswaehlen ("+s+")");
		}

		return 0.0;
	}

	public void saveYourself(PrintWriter pw) {
		String str = "";
		pw.println("////////////////// Parameter: /////////////////////////"
				+ "\n");

		pw.println("" + lampNo + "\n");
		pw.println("" + newFrom + "\n");
		pw.println("" + newTo + "\n");
		pw.println("" + intenseSlider.getValue() + "\n");
		//  pw.println(parLabel.getText()+"\n");
		//  pw.println(cbox.isSelected()+"\n");
		//optionButtonPane.saveYourself(pw);
	}

	public String saveYourself() {
		String str = "";
		str += "////////////////// Parameter: /////////////////////////\n";
		str += ("" + lampNo + "\n");
		str += ("" + newFrom + "\n");
		str += ("" + newTo + "\n");
		str += ("" + intenseSlider.getValue() + "\n");
		//  str += (parLabel.getText()+"\n");
		//  str += (cbox.isSelected()+"\n");
		return (str);
	}
} // end of class OneParameter

