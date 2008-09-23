////////////////////////////////////////////////////////////////////////
//
// This file OneParameter.java is part of SURFEX.
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
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.JTextField;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

//////////////////////////////////////////////////////////////
//
// class OneParameter
//
//////////////////////////////////////////////////////////////

public class OneParameter extends JPanel {
    
    // Anfang Variablen
    
    Project project;
    
    public int parNo = 0;
    
    public JCheckBox cbox = new JCheckBox("", false);
    
    // GUI
    
    public JLabel nameLabel = null;
    
    public double oldFrom = 0.0;
    
    public double oldTo = 1.0;
    
    public double newFrom = 0.0;
    
    public double newTo = 1.0;
    
    public JTextField from = new JTextField("0.00000");
    
    public JTextField to = new JTextField("1.00000");
    
    public JLabel parLabel = new JLabel("0.50000");
    
    JLabel fromLabel = new JLabel("from:");
    
    JLabel toLabel = new JLabel("to:");
    
    JSlider parSlider = null;

    public JLabel fctLabel = null;    
    public JTextField fct = new JTextField("");    

    // Ende Variablen
    
    // Konstruktor
    OneParameter(Project pro) {
	project = pro;
	
	setLayout(new BorderLayout());
	JPanel flowPanel = new JPanel();
	JPanel parText = new JPanel(new FlowLayout());
	
	//  flowPanel.add(cbox);
	
	nameLabel = new JLabel("p" + parNo);
	fctLabel = new JLabel("  "+nameLabel.getText()+":=");
	fct.setText("     "+nameLabel.getText()+"  ");
	fct.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent evt) {
		    fct.setText(fct.getText());
		    repaint();
		}
	    });

	flowPanel.add(nameLabel);
	
	parSlider = new JSlider(0, 1000);
	parSlider.setValue(500);
	parSlider.setMinorTickSpacing(10);
	parSlider.setMajorTickSpacing(100);
	parSlider.setPaintTicks(true);
	parSlider.addChangeListener(new ChangeListener() {
		public void stateChanged(ChangeEvent evt) {
		    updateparSliderAndLabel();
		}
	    });
	
	flowPanel.add(new JLabel("   "));
	
	flowPanel.add(fromLabel);
	flowPanel.add(from);
	from.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent evt) {
		    try {
			double tmp = Double.parseDouble(from.getText());
			if (tmp <= oldTo) {
			    tmp = Math.round(tmp * 1000) / 1000.0;
//			    System.out.println("newFrom:"+tmp);
			    oldFrom = tmp;
			    newFrom = tmp;
			    updateparSliderAndLabel();
			} else {
			    from.setText("" + oldFrom);
			}
		    } catch (Exception nanEx) {
			from.setText("" + oldFrom);
			//      System.out.println("oldFrom:"+oldFrom);
		    }
		}
	    });
	to.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent evt) {
		    try {
			double tmp = Double.parseDouble(to.getText());
			if (tmp >= oldFrom) {
			    tmp = Math.round(tmp * 1000) / 1000.0;
//			    System.out.println("newTo:"+tmp);
			    oldTo = tmp;
			    newTo = tmp;
			    updateparSliderAndLabel();
			} else {
			    to.setText("" + oldTo);
			}
		    } catch (Exception nanEx) {
			to.setText("" + oldTo);
			//      System.out.println("oldTo:"+oldTo);
		    }
		}
	    });
	
	flowPanel.add(toLabel);
	flowPanel.add(to);
	
	flowPanel.add(parSlider);
	flowPanel.add(parLabel);

	flowPanel.add(fctLabel);

//	add(parText, BorderLayout.CENTER);	
//	parText.add(fctLabel);
	add(fct);

	add(flowPanel, BorderLayout.WEST);

//	add(new JLabel(""), BorderLayout.CENTER);

    }
    
    public double getValue(){
// 	System.out.println("pS:"+parSlider.getValue()+
// 			   ", min:"+parSlider.getMinimum()+
// 			   ", max:"+parSlider.getMaximum());
	return this.parSlider.getValue();//*(parSlider.getMaximum()-parSlider.getMinimum())+parSlider.getMaximum();
    }
    
    public void updateparSliderAndLabel() {
//	System.out.println("from:"+from.getText()+
//			   "to:"+to.getText()+
//			   ", newFrom:"+newFrom+
//			   ", newTo:"+newTo);
// 	System.out.println("pS:"+parSlider.getValue()+
// 			   ", min:"+parSlider.getMinimum()+
// 			   ", max:"+parSlider.getMaximum());
	parSlider.setMinimum((int)(newFrom*1000));
	parSlider.setMaximum((int)(newTo*1000));
// 	System.out.println("pS:"+parSlider.getValue()+
// 			   ", min:"+parSlider.getMinimum()+
// 			   ", max:"+parSlider.getMaximum());
	//parSlider.setM
	parSlider.setMinorTickSpacing((int)(10*(newTo-newFrom)));
	parSlider.setMajorTickSpacing(((int)(100*(newTo-newFrom))));
	parLabel.setText(""+(parSlider.getValue() / 1000.0));
	this.repaint();
    }
    
    public boolean isSelected() {
	return (cbox.isSelected());
    }
    
    public void setParNo(int no) {
	parNo = no;
	nameLabel.setText("p" + parNo);
	fctLabel.setText("  "+nameLabel.getText()+":=");
	fct.setText(nameLabel.getText());
    }
    
    public void updateActionCommands(int internalCunr) {
    }
    
    public String getSurfCode() {
	String str = "double " + nameLabel.getText() + "=" + parLabel.getText()
	    + ";\n";
	str += nameLabel.getText()+"="+fct.getText()+";\n";
//		System.out.println(str);
	return (str);
    }
    
    public String getSurfCode(double runningValue) {
	String str = "double " + nameLabel.getText() + "=" +runningValue
	    + ";\n";
	str += nameLabel.getText()+"="+fct.getText()+";\n";
	return (str);
    }
    
    public void saveYourself(PrintWriter pw) {
	String str = "";
	pw.println("////////////////// Parameter: /////////////////////////"
		   + "\n");
	
	pw.println("" + parNo + "\n");
	pw.println("" + newFrom + "\n");
	pw.println("" + newTo + "\n");
	pw.println("" + parSlider.getValue() + "\n");
	pw.println("" + fct.getText() + "\n");
	//  pw.println(parLabel.getText()+"\n");
	//  pw.println(cbox.isSelected()+"\n");
	//optionButtonPane.saveYourself(pw);
    }
    
    public String saveYourself() {
	String str = "";
	str += "////////////////// Parameter: /////////////////////////\n";
	str += ("" + parNo + "\n");
	str += ("" + newFrom + "\n");
	str += ("" + newTo + "\n");
	str += ("" + parSlider.getValue() + "\n");
	str += ("" + fct.getText() + "\n");
	//  str += (parLabel.getText()+"\n");
	//  str += (cbox.isSelected()+"\n");
	return (str);
    }
} // end of class OneParameter

