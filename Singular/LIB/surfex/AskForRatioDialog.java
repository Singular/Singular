////////////////////////////////////////////////////////////////////////
//
// This file AppearanceScheme.java is part of SURFEX.
// 
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
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

import java.awt.*;
import javax.swing.*;
/*
 * Created on 25.03.2006
 *
 * TODO To change the template for this generated file go to
 * Window - Preferences - Java - Code Style - Code Templates
 */

/**
 * @author stephan
 *
 * TODO To change the template for this generated type comment go to
 * Window - Preferences - Java - Code Style - Code Templates
 */
public class AskForRatioDialog extends JFrame{
	
	AskForRatioDialog(double defaultratio){
		super("Ratio Dialog");
		setSize(200, 200);
		setLocation(100, 100);
	    Container cp = getContentPane();
	    cp.setLayout(new BorderLayout());
	    JPanel panel = new JPanel(new GridLayout(1, 1));
	    cp.add(panel);
	    panel.add(new JLabel("ratio: "));
	    JTextField T=new JTextField(defaultratio+"");
	    panel.add(T);
	    JButton Default=new JButton("Default");
	    panel.add(Default);
	    JButton OK =new JButton("OK");
	    panel.add(OK);
	    
	
	}

}
