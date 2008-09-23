////////////////////////////////////////////////////////////////////////
//
// This file SurfexStartFrame.java is part of SURFEX.
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

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

public class SurfexStartFrame extends JPanel
                             implements ActionListener {
    public final static int ONE_SECOND = 1000;

    private JProgressBar progressBar;
    private Timer timer;
    private JButton startButton;
   // private LongTask task;
    private JLabel taskOutput;
    private String newline = "\n";


    public SurfexStartFrame(int LengthOfTask) {
        super(new BorderLayout());
        //task = new LongTask();

        progressBar = new JProgressBar(0, LengthOfTask);
        progressBar.setValue(0);
        progressBar.setStringPainted(true);

        taskOutput = new JLabel("                         ");
//        taskOutput.setEditable(false);

        JPanel panel = new JPanel();
        panel.add(progressBar);

	ImageIcon pic = new ImageIcon("images/classify_no19.jpg");
	JLabel labPic = new JLabel(pic);

        add(panel, BorderLayout.CENTER);
	add(labPic, BorderLayout.NORTH);
        add(taskOutput, BorderLayout.SOUTH);
        setBorder(BorderFactory.createEmptyBorder(20, 20, 20, 20));
    }

    /**
     * Called when the user presses the start button.
     */
    public void actionPerformed(ActionEvent evt) {
        startButton.setEnabled(false);
        setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
     //   task.go();
        timer.start();
    }

    /**
     * Create the GUI and show it.  For thread safety,
     * this method should be invoked from the
     * event-dispatching thread.
     */
    private static void createAndShowGUI() {
        //Make sure we have nice window decorations.
        JFrame.setDefaultLookAndFeelDecorated(true);

        //Create and set up the window.
        JFrame frame = new JFrame("ProgressBarDemo");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        //Create and set up the content pane.
        JComponent newContentPane = new ProgressFrame(100);
        newContentPane.setOpaque(true); //content panes must be opaque
        frame.setContentPane(newContentPane);

        //Display the window.
        frame.pack();
        frame.setVisible(true);
    }

    public void refresh(int progress, String Message){
	progressBar.setValue(progress);
//    		      System.out.println(progress+Message);
	String s = Message;
	if (s != null) {
	    taskOutput.setText(s);
	    taskOutput.repaint();
//	    taskOutput.append(s + newline);
//	    taskOutput.setCaretPosition(
//		taskOutput.getDocument().getLength());
	}
    }
}

