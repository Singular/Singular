////////////////////////////////////////////////////////////////////////
//
// This file ParameterAdmin.java is part of SURFEX.
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
import java.util.*;

import javax.swing.*;
import javax.swing.SwingUtilities;

//////////////////////////////////////////////////////////////
//
// class ParameterAdmin
//
//////////////////////////////////////////////////////////////

public class ParameterAdmin extends JPanel implements ActionListener {

	JPanel parPanel;

	JComboBox paramComboBox; // wird fuer den SaveMovieDialog gebraucht
	
	public Vector parList = new Vector();

	Project project;

	surfex surfex_;

	ParameterAdmin(surfex su, Project pro) {
		surfex_ = su;
		project = pro;
		
		paramComboBox=new JComboBox();

		setLayout(new BorderLayout());
		parPanel = new JPanel(new GridLayout(20, 0));
		add(parPanel, BorderLayout.CENTER);
	}

	public void actionPerformed(ActionEvent e) {
	}

    public void clear() {
	parList.clear();
	parPanel.removeAll();
//	parcounter = 0;
    }

	public String[] getAllParams(){
		String[] s=new String[this.parList.size()];
		ListIterator li = parList.listIterator();
		OneParameter tmpParameter;
		int i=0;
		
		while (li.hasNext()) {
			tmpParameter=((OneParameter)li.next());
			s[i]=tmpParameter.nameLabel.getText();
			
			
			i++;
		}	
		return s;
	}
	
    public double[] getAllParamValues(){
	double[] s=new double[this.parList.size()];
	ListIterator li = parList.listIterator();
	OneParameter tmpParameter;
	int i=0;
	
	while (li.hasNext()) {
	    tmpParameter=((OneParameter)li.next());
	    s[i]=tmpParameter.getValue()/1000.0;
	    
	    i++;
	}	
	return s;
    }
    
	
	
	
	public void newParameter() {
		//  System.out.println("newParameter");
		OneParameter par = new OneParameter(project);

		// find a non-used parameter number:
		int n = parList.size() + 1;
		ListIterator li = parList.listIterator();
		while (li.hasNext()) {
			if (n == ((OneParameter) (li.next())).parNo) {
				// if the parameter number n is already in use
				n = n + 1; // try the next number
				// reset the list iterator to the first element in the list:
				while (li.hasPrevious()) {
					li.previous();
				}
			}
		}
		par.setParNo(n);
		//  System.out.println("parNo:"+n);

		// add this parameter to the list:
		parList.add(par);
		//  System.out.println("parList:"+parList.size());
		parPanel.add(par);
		
		
		// rausfinden, wo man den Parameter in ComboBox einfuegen kann:
		int i=0;
		//System.out.;
		for(i=0;i<paramComboBox.getItemCount();i++){
			if(((String)paramComboBox.getItemAt(i)).compareTo(par.nameLabel.getText())>=0){
		//		i--;
				break;
			}
		}
		//i--;
//		System.out.println("index:"+i);
		paramComboBox.insertItemAt(par.nameLabel.getText(),i);
		
		if(project.saveMovieDialog!=null){
			//nur updaten, falls es ihn ueberhaupt gibt...
			SwingUtilities.updateComponentTreeUI(project.saveMovieDialog);
		}
		SwingUtilities.updateComponentTreeUI(this);
	}

	public String save() {
		String str = "";
		ListIterator li = parList.listIterator();
		str += "////////////////// PARAMETERS: /////////////////////////"
				+ "\n";
		str += parList.size() + "\n";
		while (li.hasNext()) {
			str += ((OneParameter) li.next()).saveYourself();
		}
		return (str);
	}

    public void loadParameters(BufferedReader bs, String datatype,
				int iVersion) {
	int k;
	try {
	    String nix = bs.readLine();
	    String zeile1;
	    boolean first = true;
	    
	    for (k = Integer.parseInt(bs.readLine()); k > 0; k--) {
		zeile1 = bs.readLine();//) != null) {
		newParameter();
		if (datatype.equals("specify")) {
		    String strline;
		    try {
			bs.mark(255);
			strline = bs.readLine();
			while ((strline.equals("parno:"))
			       || (strline.equals("fromtoval:"))
			       || (strline.equals("function:"))) {
			    //          System.out.println("strline:"+strline);
			    if (strline.equals("parno:")) {
				((OneParameter) parList.lastElement())
				    .setParNo(Integer.parseInt(bs
							       .readLine()));
			    }
			    if (strline.equals("function:")) {
				((OneParameter) parList.lastElement())
				    .fct.setText(bs.readLine());
			    }
			    if (strline.equals("fromtoval:")) {
				((OneParameter) parList.lastElement()).oldFrom = Double
				    .parseDouble(bs.readLine());
				((OneParameter) parList.lastElement()).newFrom = ((OneParameter) parList
										  .lastElement()).oldFrom;
				
				((OneParameter) parList.lastElement()).oldTo = Double
				    .parseDouble(bs.readLine());
				((OneParameter) parList.lastElement()).newTo = ((OneParameter) parList
										.lastElement()).oldTo;
				
				((OneParameter) parList.lastElement()).parSlider
				    .setMinimum((int)(((OneParameter) parList.lastElement()).newFrom*1000.0));
				((OneParameter) parList.lastElement()).parSlider
				    .setMaximum((int)(((OneParameter) parList.lastElement()).newTo*1000.0));
				
				((OneParameter) parList.lastElement()).parSlider
				    .setValue(Integer.parseInt(bs
							       .readLine()));
				((OneParameter) parList.lastElement())
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
		    ((OneParameter) parList.lastElement())
			.setParNo(Integer.parseInt(bs.readLine()));
		    ((OneParameter) parList.lastElement()).oldFrom = Double
			.parseDouble(bs.readLine());
		    ((OneParameter) parList.lastElement())
			.newFrom = ((OneParameter) parList.lastElement()).oldFrom;
		    ((OneParameter) parList.lastElement()).oldTo = Double
			.parseDouble(bs.readLine());
		    ((OneParameter) parList.lastElement()).newTo = 
			((OneParameter) parList.lastElement()).oldTo;
		    ((OneParameter) parList.lastElement()).parSlider
			.setMinimum((int)(((OneParameter) parList
					   .lastElement()).newFrom*1000.0));
		    ((OneParameter) parList.lastElement()).parSlider
			.setMaximum((int)(((OneParameter) parList
					   .lastElement()).newTo*1000.0));
		    ((OneParameter) parList.lastElement()).parSlider
			.setValue(Integer.parseInt(bs.readLine()));
		    ((OneParameter) parList.lastElement()).from
			.setText("" + ((OneParameter) 
				       parList.lastElement()).newFrom);
		    ((OneParameter) parList.lastElement()).to
			.setText("" + ((OneParameter) parList.lastElement()).newTo);
		    ((OneParameter) parList.lastElement())
			.updateparSliderAndLabel();

		    if(iVersion>=8906) {
			((OneParameter) parList.lastElement()).fct
			    .setText(bs.readLine());
		    }

		    ((OneParameter) parList.lastElement()).repaint();
		}
		if (datatype.equals("complete") || datatype.equals("onlystyle")) {
		    //        ((OneParameter) parList.lastElement()).cbox.setSelected((Boolean.valueOf(bs.readLine())).booleanValue());
		}
	    }
	} catch (IOException e) {
	    System.out.println(e);
	}
    }
    
    /*public Vector getParameterNames(){
      Vector l = new Vector();//new String[parList.size()];
      
      ListIterator li = parList.listIterator();
      while (li.hasNext()) {
      l.add(((OneParameter)li.next()).nameLabel.getText()) ;
      //i++;
      }		
      return l;
      }*/
    
    public JComboBox getParamComboBox(){
	return paramComboBox;
	
    }
    
    public OneParameter getParameter(String s){
	ListIterator li = parList.listIterator();
		OneParameter tmpParameter;
		while (li.hasNext()) {
			tmpParameter=((OneParameter)li.next());
			if(tmpParameter.nameLabel.getText().compareTo(s)==0){
				return tmpParameter;
			}
			
			
			//i++;
		}		
		return null;
	}
	
	
	
} // end class ParameterAdmin


