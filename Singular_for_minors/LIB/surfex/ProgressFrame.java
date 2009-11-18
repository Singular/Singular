////////////////////////////////////////////////////////////////////////
//
// This file ProgressFrame.java is part of SURFEX.
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

public class ProgressFrame extends JPanel
                             implements ActionListener {
    public final static int ONE_SECOND = 1000;

    private JProgressBar progressBar;
    private Timer timer;
    private JButton startButton;
   // private LongTask task;
    private JTextArea taskOutput;
    private String newline = "\n";
    
    public boolean processCanceled=false;

    JButton playMovieButton=new JButton("play Movie");

    JButton cancelButton=new JButton("cancel");

    JButton closeButton=new JButton("close");
    
    JLabel previewLabel=new JLabel(new ImageIcon("prevstart.jpg"));

    public ProgressFrame(int LengthOfTask) {
        super(new BorderLayout());
        //task = new LongTask();


        ImageIcon i=new ImageIcon();
        
        progressBar = new JProgressBar(0, LengthOfTask);
        progressBar.setValue(0);
        progressBar.setStringPainted(true);

        taskOutput = new JTextArea(5, 20);
        taskOutput.setMargin(new Insets(5,5,5,5));
        taskOutput.setEditable(false);
        taskOutput.setCursor(null); //inherit the panel's cursor
                                    //see bug 4851758

        //Create and set up the content pane.
        JPanel pane=new JPanel(new BorderLayout());
        JPanel progressPanel = new JPanel(new BorderLayout());
        progressPanel.add(progressBar,BorderLayout.NORTH);
        progressPanel.add(new JScrollPane(taskOutput),BorderLayout.CENTER);
      //  progressPanel.setBorder(BorderFactory.createEmptyBorder(20, 20, 20, 20));

        pane.add(progressPanel,BorderLayout.NORTH);
        JPanel previewPanel =new JPanel(new BorderLayout());
        previewPanel.add(new JLabel("last frame:"),BorderLayout.WEST);
        //previewLabel.setSize(100,100);
        previewPanel.add(new JScrollPane(previewLabel),BorderLayout.CENTER);
        pane.add(previewPanel,BorderLayout.CENTER);
    //    System.out.println("dsfkgreg--------------");
        JPanel controlPanel=new JPanel(new GridLayout(1,3));
        
        playMovieButton.setEnabled(false);
        controlPanel.add(playMovieButton); 
        cancelButton.addActionListener(new ActionListener(){
        	public void actionPerformed(ActionEvent ae){
        		processCanceled=true;
        	}
        	
        });
        controlPanel.add(cancelButton);
    //    controlPanel.add(closeButton);
        pane.add(controlPanel,BorderLayout.SOUTH);
       // newContentPane.setOpaque(true); //content panes must be opaque
        //frame.setContentPane(pane);
        
        
        //JPanel panel = new JPanel();
        //panel.add(progressBar);

        add(pane, BorderLayout.CENTER);
       //add(new JScrollPane(taskOutput), BorderLayout.CENTER);
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



    public void refresh(int progress, String Message){
	progressBar.setValue(progress);
//	System.out.println("---");
	String s = Message;
	if (s != null) {
	    taskOutput.append(s + newline);
	    taskOutput.setCaretPosition(
		taskOutput.getDocument().getLength());
	}
	
    }
    
    public void refresh(int progress, String Message, String prevFileLoc){
	      refresh(progress, Message);
	     // previewLabel.setText(prevFileLoc);
	      previewLabel.setIcon(new ImageIcon(prevFileLoc));
//	      System.out.println("preview update");
    }
    
}

