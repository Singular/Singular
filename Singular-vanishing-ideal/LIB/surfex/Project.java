////////////////////////////////////////////////////////////////////////
//
// This file Project.java is part of SURFEX.
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
import java.awt.Panel;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLConnection;
import java.net.URLEncoder;
import java.util.Hashtable;
import java.util.Random;
import java.util.Vector;

import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JColorChooser;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JSlider;
import javax.swing.JSplitPane;
import javax.swing.JTextField;
import javax.swing.SwingUtilities;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.filechooser.FileFilter;

//////////////////////////////////////////////////////////////
//
//class Project
//
//////////////////////////////////////////////////////////////
public class Project extends JPanel implements ActionListener {
  surfex surfex_;

  public jv4surfex jv4sx;

  public int ProjectNumber;

  public String projectName;

  String old_tmpsubdir = "";
 
  String old_surfCodePart1 = "", old_surfCodePart2 = "";
 
  public AppearanceScheme appearanceScheme = new AppearanceScheme();

  public boolean saveDone = false;

  JTextField tf;

  LampAdmin lampAdmin;
  public String filename;

  SavePicDialog savePicDialog;
  
  SaveMovieDialog saveMovieDialog;

  RayFrame rayFrame;

  Vector tbuttons = new Vector();

  JButton bt1 = new JButton();

  JButton bt2 = new JButton();

  JButton bt3 = new JButton();

  JButton bt4 = new JButton();

  JButton bt5 = new JButton();

  JButton bt6 = new JButton();

  JButton bt7 = new JButton();
  
  int rand;

  JCheckBox raytraceAlways = new JCheckBox("permanently", true);

  JCheckBox antialiasing = new JCheckBox("antialiasing", false);

  JButton bgColorButton = null;

  JComboBox clipMode = new JComboBox();

  JTextField clipRadius = new JTextField("9.0");

    double scale = 1.0;
    
    String rootFinder = "bezier_all_roots";
    String epsilon = "0.00000000000001";

    int interpolConst = 2;
    JComboBox interpolConstBox = null; 

//  JSlider scaleSlider = null;

//  JLabel scaleLabel = null;
    
    JPanel controlpanel = new JPanel();
    
    EquationAdmin eqAdm;
    
    CurveAdmin cuAdm;
    
    ParameterAdmin parAdmin;
    
    public SolitaryPointsAdmin solPtsAdm;
    
    String tmp_surfex_file = "tmp_surfex.jpg";
    
    Project(surfex su, jv4surfex jv4sx, int proNr, String proName) {    
	surfex_ = su;
	rayFrame = surfex_.rayFrame;
	this.jv4sx = jv4sx;
	ProjectNumber = proNr;
	projectName = proName;
	
	// create a new temporary filename: tmp_surfex_file
	Random myRNG = new Random();
	int myRN = myRNG.nextInt(99999999);
	//System.out.println("rn:"+myRN);
	tmp_surfex_file = "tmp_surfex_" + myRN + "_" + proNr + ".jpg";
	
	savePicDialog = new SavePicDialog(projectName, rayFrame, this, surfex_);
	
	
	// int i, String filename, RayFrame ray, surfex su,
	// jv4surfex jv4sx
	eqAdm = new EquationAdmin(proNr, "", surfex_.rayFrame, surfex_,
				  surfex_.jv4sx, this);
	cuAdm = new CurveAdmin(proNr, "", surfex_, this);
	
	setLayout(new BorderLayout());//new GridLayout(3,1));
	JPanel toplinePanel = new JPanel(new BorderLayout());
	JPanel toplineGridPanel = new JPanel(new GridLayout(2, 1));
	JPanel controlpanel = new JPanel(new FlowLayout());
	JPanel globalConfigPanel = new JPanel(new FlowLayout());
	JPanel leftFlushGCPanel = new JPanel(new BorderLayout());
	
	
	toplinePanel.add(toplineGridPanel, BorderLayout.WEST);
	toplineGridPanel.add(controlpanel);
	leftFlushGCPanel.add(globalConfigPanel, BorderLayout.WEST);
	toplineGridPanel.add(leftFlushGCPanel);
	
	
	add(toplinePanel, BorderLayout.NORTH);
	
	parAdmin = new ParameterAdmin(surfex_, this);
	JSplitPane tempPanel = new JSplitPane(JSplitPane.VERTICAL_SPLIT,
					      new JScrollPane(eqAdm), new JScrollPane(cuAdm));
	tempPanel.setDividerLocation(160);
	JSplitPane tempPanel1 = new JSplitPane(JSplitPane.VERTICAL_SPLIT,
					       new JScrollPane(parAdmin), tempPanel);
	tempPanel1.setDividerLocation(60);
	//add(tempPanel1,BorderLayout.CENTER);
	//tempPanel.add(new JScrollPane(eqAdm));
	//tempPanel.add(new JScrollPane(cuAdm));
	
	solPtsAdm = new SolitaryPointsAdmin(surfex_, this);
	JSplitPane tempPanel2 = new JSplitPane(JSplitPane.VERTICAL_SPLIT,
					       tempPanel1, new JScrollPane(solPtsAdm));
	tempPanel2.setDividerLocation(300);
	add(tempPanel2, BorderLayout.CENTER);
	
	//JPanel controlpanel=new JPanel();
	
	//this.add(controlpanel, BorderLayout.NORTH);
	
	JButton closeProject;
	try {
	    if ((surfex_.filePrefix).equals("")) {
		closeProject = new JButton(new ImageIcon("images/closeProject.gif"));
	    } else {
		closeProject = new JButton(new ImageIcon(new URL(
							     surfex_.filePrefix + 
							     "images/closeProject.gif")));
	    }
	} catch (MalformedURLException e) {
	    System.out.println(e.toString());
	    closeProject = new JButton();
	}
	closeProject.setToolTipText("close this project");
	closeProject.addActionListener(this);
	closeProject.setActionCommand("closeProject");
	if (!surfex_.inAnApplet) {
//	    controlpanel.add(closeProject);
	}
	
	// some space:
	controlpanel.add(new JLabel("   "));
	
	JButton save;
	try {
	    if ((surfex_.filePrefix).equals("")) {
		save = new JButton(new ImageIcon("images/save.gif"));
	    } else {
		save = new JButton(new ImageIcon(new URL(surfex_.filePrefix
							 + "images/save.gif")));
	    }
	} catch (MalformedURLException e) {
	    System.out.println(e.toString());
	    save = new JButton();
	}
	save.setToolTipText("save this project");
	save.addActionListener(this);
	save.setActionCommand("save");
	if (!surfex_.inAnApplet) {
	    //controlpanel.add(save);
	}
	
	JButton saveas;
	try {
	    if ((surfex_.filePrefix).equals("")) {
		saveas = new JButton(new ImageIcon("images/saveas.gif"));
	    } else {
		saveas = new JButton(new ImageIcon(new URL(surfex_.filePrefix
							   + "images/saveas.gif")));
	    }
	} catch (MalformedURLException e) {
	    System.out.println(e.toString());
	    saveas = new JButton();
	}
	saveas.setToolTipText("save this Project as...");    
	
	saveas.addActionListener(this);
	saveas.setActionCommand("saveas");
	//if(!surfex_.inAnApplet) {
	controlpanel.add(saveas);
	//}
	
	// button1:
	bt3.setText("your files");
	bt3.setToolTipText("show your files in a new browser window");
	//    getContentPane().add(bt2);
	bt3.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent evt) {
		    bt3ActionPerformed(evt);
		}
	    });
	if (surfex_.inAnApplet) {
	    controlpanel.add(bt3);
	}
	
	JButton savePic;
	try {
	    if ((surfex_.filePrefix).equals("")) {
		savePic = new JButton(new ImageIcon("images/savePic.gif"));
	    } else {
		savePic = new JButton(new ImageIcon(new URL(surfex_.filePrefix
							    + "images/savePic.gif")));
	    }
	} catch (MalformedURLException e) {
	    System.out.println(e.toString());
	    savePic = new JButton();
	}
	savePic.setToolTipText("save this raytraced picture as...");
	savePic.addActionListener(this);
	savePic.setActionCommand("savePic");
	controlpanel.add(savePic);
	
	JButton saveMovie;
	try {
	    if ((surfex_.filePrefix).equals("")) {
		saveMovie = new JButton(new ImageIcon("images/saveMovie.gif"));
	    } else {
		saveMovie = new JButton(new ImageIcon(new URL(surfex_.filePrefix
							      + "images/saveMovie.mov")));
	    }
	} catch (MalformedURLException e) {
	    System.out.println(e.toString());
	    saveMovie = new JButton();
	}
	saveMovie.setToolTipText("create and save a movie of this surface as...");
	saveMovie.addActionListener(this);
	saveMovie.setActionCommand("saveMovie");
	controlpanel.add(saveMovie);
	
	JButton lampButt;
	try {
	    if ((surfex_.filePrefix).equals("")) {
		lampButt = new JButton(new ImageIcon("images/lamps.gif"));
	    } else {
		lampButt = new JButton(new ImageIcon(new URL(surfex_.filePrefix
							     + "images/lamps.gif")));
	    }
	} catch (MalformedURLException e) {
	    System.out.println(e.toString());
	    lampButt = new JButton();
	}
	lampButt.setToolTipText("configure project's lamps");
	lampButt.addActionListener(this);
	lampButt.setActionCommand("configure lamps");
//    controlpanel.add(lampButt);
	
	// some space:
	controlpanel.add(new JLabel("   "));
	
	//
	bt6.setText("add param.");
	bt6.setToolTipText("add a parameter");
	//    getContentPane().add(bt1);
	bt6.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent evt) {
		    bt6ActionPerformed(evt);
		}
	    });
	controlpanel.add(bt6);
	
	//
	bt1.setText("add eqn.");
	bt1.setToolTipText("add an equation");
	//    getContentPane().add(bt1);
	bt1.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent evt) {
		    bt1ActionPerformed(evt);
		}
	    });
	controlpanel.add(bt1);
	
	//
	bt5.setText("add curve");
	bt5.setToolTipText("add a curve");
	//   getContentPane().add(bt1);
	bt5.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent evt) {
		    bt5ActionPerformed(evt);
		}
	    });
	controlpanel.add(bt5);
	
    //
	bt7.setText("add pt.");
	bt7.setToolTipText("add a point");
	bt7.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent evt) {
		    bt7ActionPerformed(evt);
		}
	    });
	controlpanel.add(bt7);
	
	// some space:
	controlpanel.add(new JLabel("   "));
	
	//
	bt2.setText("raytrace");
	bt2.setEnabled(false);
	bt2.setToolTipText("raytrace");
	//    getContentPane().add(bt2);
	bt2.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent evt) {
		    bt2ActionPerformed(evt);
		}
	    });
//	controlpanel.add(bt2);
	
	// 
	//controlpanel.add(new JLabel("permanently"));
	//    getContentPane().add(bt4);
	raytraceAlways.addChangeListener(new ChangeListener() {
		public void stateChanged(ChangeEvent evt) {
		    raytraceAlwaysChanged(evt);
		}
	    });
	raytraceAlways.setEnabled(false);
//	controlpanel.add(raytraceAlways);
	
	// some space:
	//controlpanel.add(new JLabel(" "));
	
	// a checkbox for activating antialiasing
	globalConfigPanel.add(antialiasing);
	
	Vector temp = new Vector();
	temp.add(new Integer(1));
	temp.add(new Integer(2));
	temp.add(new Integer(4));
	temp.add(new Integer(8));
	interpolConstBox = new JComboBox(temp);
	interpolConstBox.setSelectedIndex(1);
	interpolConstBox.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent evt) {
		    setInterpolConst(((Integer)(interpolConstBox.getSelectedItem())).intValue());
		}
	    });
	globalConfigPanel.add(new JLabel(" preview quality:"));
	globalConfigPanel.add(interpolConstBox);
	
	// a button for choosing the background color:
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
	globalConfigPanel.add(new JLabel(" background-color:"));
	globalConfigPanel.add(bgColorButton);

	clipMode.insertItemAt("sphere", 0);
	clipMode.insertItemAt("tetrahedron", 1);
	clipMode.insertItemAt("cube", 2);
	clipMode.insertItemAt("octahedron", 3);
	clipMode.insertItemAt("dodecahedron", 4);
	clipMode.insertItemAt("icosahedron", 5);
	clipMode.insertItemAt("cylinder", 6);
	clipMode.insertItemAt("user-defined", 7);
	clipMode.insertItemAt("none", 8);
	clipMode.setSelectedIndex(0);

	globalConfigPanel.add(new JLabel(" clip-mode:"));
	globalConfigPanel.add(clipMode);
	globalConfigPanel.add(new JLabel(" clip-radius:"));
	globalConfigPanel.add(clipRadius);
	
	/*   // the Slider for scaling:
	     scaleSlider = new JSlider(1, 1000);
	     scaleLabel = new JLabel("1.0");
	     scaleSlider.setValue(100);
	     scaleSlider.setMinorTickSpacing(10);
	     scaleSlider.setMajorTickSpacing(100);
	     scaleSlider.setPaintTicks(true);
	     Hashtable labelTable = new Hashtable();
	     labelTable.put(new Integer(10), new JLabel("1"));
	     labelTable.put(new Integer(500), new JLabel("5"));
	     labelTable.put(new Integer(1000), new JLabel("10"));
	     scaleSlider.setLabelTable(labelTable);
	     scaleSlider.addChangeListener(new ChangeListener() {
	     public void stateChanged(ChangeEvent evt) {
	     String str = "" + scaleSlider.getValue() / 100.0;
	     int end = 6;
	     if (str.length() <= 6) {
          end = str.length();
        }
        scaleLabel.setText(str.substring(0, end));
        scaleSliderUpdated();
      }
    });
    // we do not show the scaleSlider for the moment,
    // because it does not work yet.
    globalConfigPanel.add(new JLabel("scale:"));
    globalConfigPanel.add(scaleSlider);
    globalConfigPanel.add(scaleLabel);
*/
    // button4:
    //bt4.setText("raytrace always");
    //bt4.setToolTipText("raytrace always");
    //  controlpanel.add(bt4);


	
	rand=(int)(Math.random()*2000000000);
	
	// muss nach pqrameterAdmin instanziert werden, da auf paramComboBox zurueckgegriffen wird
	saveMovieDialog = new SaveMovieDialog(projectName, rayFrame, this, surfex_);
	
	lampAdmin=new LampAdmin(surfex_,this);
	
	
//    jv4sx.disp.getCamera().setScale(1.0);    
	jv4sx.setScale(1.0);    
//    System.out.println("Scale:" + jv4sx.getScale());
	
    }

    public void clear() {
	eqAdm.clear();
	cuAdm.clear();
	parAdmin.clear();
	solPtsAdm.clear();
	newEquation();
    }

    public void setRootFinder(String s) {
	rootFinder = s;
    }

    public String getRootFinder() {
	return(rootFinder);
    }
    
    public void setEpsilon(String s) {
	epsilon = s;
    }

    public String getEpsilon() {
	return(epsilon);
    }
    
    public double getScale() {
	return(scale);
    }

    public int getClipMode() {
	return(clipMode.getSelectedIndex());
    }

    public String getClipRadius() {
	return(clipRadius.getText());
    }
    
    public void setScale(double s) {
//	System.out.println("setScale P:"+s);
	scale = s;
//	jv4sx.setScale(s);
    }
    
    public void setInterpolConst(int c) {
	interpolConst = c;
    }

    public int getInterpolConst() {
	return(interpolConst);
    }
  
  public Vector getAllLamps(){
  	return lampAdmin.getAllLamps();
  }

  public void bt1ActionPerformed(ActionEvent evt) {
    newEquation();
  }

  public void newEquation() {
    eqAdm.newEquation();
    cuAdm.equationListChanged_add(new Integer(((Equation) eqAdm.eqnList
        .lastElement()).eqnr).toString());
    solPtsAdm.equationListChanged_add(new Integer(((Equation) eqAdm.eqnList
        .lastElement()).eqnr).toString());
   }

  public void bt6ActionPerformed(ActionEvent evt) {
    newParameter();
  }
  
  public boolean eqauls(Project p){
  	if(p!=null){
  		return (this.rand==p.rand);
  		
  	}else{
  		System.out.println("error in project.equals(project p):Nullpinter");
  	}
  	return false;
  	
  }

  public void newParameter() {
    parAdmin.newParameter();
  }

  public void newCurve() {
    cuAdm.newCurve();
  }

  public void bt2ActionPerformed(ActionEvent evt) {
    // raytracen
    String fn = surfex_.tmpDir + "test.png";
    if (surfex_.configFrame.surf.isSelected()) {
      fn = surfex_.tmpDir + tmp_surfex_file;
    } else {
      fn = surfex_.tmpDir + "test.png";
    }
    SavePic saveThread = new SavePic(fn, false, antialiasing.isSelected(),
        rayFrame.getContentPane().getSize().height
            / getInterpolConst(), rayFrame.getContentPane()
            .getSize().width
            / getInterpolConst(), 72, surfex_, this, this.jv4sx.getCamPos(),this.jv4sx.getViewDir(),this.jv4sx.getUpVector(),this.jv4sx.getRightVector(),parAdmin.getAllParams(),parAdmin.getAllParamValues(),this.jv4sx,this.getAllLamps());
    try {
      saveThread.start();
      saveThread.join();
    } catch (InterruptedException e) {

    }
    // thread ist fertig
    //System.out.println("finally");
    if (surfex_.OS == surfex_.osWINDOWS) {
      Runtime r = Runtime.getRuntime();
      Process p;
      try {
        if (getInterpolConst() != 1) {
//          System.out.println("resample");
          p = r.exec(surfex_.configFrame.iviewPath.getText() + " "
              + surfex_.tmpDir + "test.png /resample=(0,"
              + rayFrame.getContentPane().getSize().height
              + ") /convert=" + surfex_.tmpDir + "test.png");
        } else {

          p = r.exec("iview\\i_view32.exe " + surfex_.tmpDir
              + "test.png /convert=" + surfex_.tmpDir
              + "test.png");

        }
        p.waitFor();
        eqAdm.img_filename = surfex_.tmpDir + "test.png";
      } catch (Exception er) {
        System.out.println(er);
      }
    }
    if (!rayFrame.isVisible()) {
      //System.out.println("setVisible rayFrame");
      //rayFrame.setVisible(true);
    }
    rayFrame.changeBackground(eqAdm.img_filename);

    SwingUtilities.updateComponentTreeUI(bt4);
  } // end of bt2ActionPerformed()

  public void bt3ActionPerformed(ActionEvent evt) {
    // open the temporary directory for the current project
    // in a new browser window
    try {
      surfex_.theApplet.getAppletContext().showDocument(
          new URL(surfex_.theApplet.getCodeBase() + "/"
              + old_tmpsubdir), "_blank");
    } catch (Exception e) {
      System.out.println(e.toString());
    }
  } // end of bt3ActionPerformed()

  public void bt4ActionPerformed(ActionEvent evt) {
  }

  public void startRaytraceAlways() {
    surfex_.configFrame.last[4] = true;
    double ang[] = this.eqAdm.getAngles();
    double scale[] = this.eqAdm.getScales();
    surfex_.newUpdateRayframeImmediatlyThread();
    surfex_.updateRayframeImmediatlyThread.start();
  }

  public void raytraceAlwaysChanged(ChangeEvent evt) {
    if (raytraceAlways.isSelected() && !surfex_.configFrame.last[4]
        && surfex_.pane.getSelectedIndex() >= 0) {
      startRaytraceAlways();
    }
    if (!raytraceAlways.isSelected() && surfex_.configFrame.last[4]) {
      surfex_.configFrame.last[4] = false;
      surfex_.updateRayframeImmediatlyThread.stop();
    }
  } // end of bt4ActionPerformed()

  public void bt5ActionPerformed(ActionEvent evt) {
    cuAdm.newCurve();
  } // end of bt5ActionPerformed()

  public Lamp getSelectedLamp(){
  	if(lampAdmin==null){
  		System.out.println("lampAdmin null");
  	}
  	return lampAdmin.getSelectedLamp();
  }
  public void bt7ActionPerformed(ActionEvent evt) {
    solPtsAdm.newSolitaryPoint();
  } // end of bt7ActionPerformed()

  public void closeProject() {
    surfex_.closeProject(ProjectNumber);
  }

  private void save_as_and_close_Project() {
    String internalname = projectName; // interneer name des aktuell foku
    // creating new Frame
    final JFrame frame2 = new JFrame("save" + internalname + " as");

    // adding container
    Container contentPane = frame2.getContentPane();

    // creating Filechooser
    JFileChooser fileChooser = new JFileChooser(surfex_.currentDirectory);

    // adding filechooser
    contentPane.add(fileChooser, BorderLayout.CENTER);

    // Create ActionListener
    ActionListener actionListener = new ActionListener() {
      public void actionPerformed(ActionEvent actionEvent) {
        JFileChooser theFileChooser = (JFileChooser) actionEvent.getSource();

        // get command
        String command = actionEvent.getActionCommand();

        // check if doubleclickt or if "open" was pressed
        if (command.equals(JFileChooser.APPROVE_SELECTION)) {

          File selectedFile = theFileChooser.getSelectedFile();

          // making savepopup invisible/removing it
          frame2.dispose();

          // creating complete filepath
          String filelocation = new String(selectedFile.getParent()
              + "\\" + selectedFile.getName());

          // ausgabe fehler bein z.B.: d:\ -> d:\\ bzw \ -> \\
//          System.out.println("filelocation : " + filelocation + "\n");
          save(filelocation);
          // this is not the cancel-button
          // } else if (command.equals(JFileChooser.CANCEL_SELECTION))
          // {
        }
        closeProject();
      }
    };

    // Adding filefilters
    // how to get "all common files"&"common image files" in current
    // language?
    // no global array! -> you have to carry the readmenuentries[] with you
    // through
    // all maethods as argument @aufruf (?selection?)
    FileFilter acfFilter = new ExtensionFileFilter("all common files",
        new String[] { "JPG", "JPEG", "jpg", "jpeg",
        //"gif", "tif",
            //"spc",
//            "sun" 
	});
    fileChooser.addChoosableFileFilter(acfFilter);

    FileFilter figFilter = new ExtensionFileFilter("*.fig",
        new String[] { "fig" });
    //fileChooser.addChoosableFileFilter(figFilter);

    FileFilter suxFilter = new ExtensionFileFilter("*.spc (spicy-file)",
        new String[] { "spc" });
    //fileChooser.addChoosableFileFilter(suxFilter);

    // adding actionListener
    fileChooser.addActionListener(actionListener);
    frame2.pack();
    frame2.setVisible(true);
  }

  public void showSavePicDialog() {
    if (antialiasing.isSelected()) {
      savePicDialog.colorType.setSelectedIndex(2);//.setSelected(true);
    } else {
      savePicDialog.colorType.setSelectedIndex(0);//antialiasing.setSelected(false);
    }
    savePicDialog.setVisible(true);
  }
  
  public void showSaveMovieDialog() {
      if (antialiasing.isSelected()) {
	  saveMovieDialog.antialiasingQuality.setSelectedIndex(5);
      } else {
	  saveMovieDialog.antialiasingQuality.setSelectedIndex(0);
      }
      surfex_.showMoviePreview = true;
      saveMovieDialog.setVisible(true);
  }
    
    public String getSuxFile() {
	String str = "";
    str += "this is surfex v" + surfex_.strVersion + "\n";
    str += "///////// TYPE: //////" + "\n";
    str += "complete\n";
    str += "///////// GENERAL DATA: //////" + "\n";
    str += jv4sx.disp.getCamera().getUpVector().getEntry(0) + "\n";
    str += jv4sx.disp.getCamera().getUpVector().getEntry(1) + "\n";
    str += jv4sx.disp.getCamera().getUpVector().getEntry(2) + "\n";
    str += jv4sx.disp.getCamera().getViewDir().getEntry(0) + "\n";
    str += jv4sx.disp.getCamera().getViewDir().getEntry(1) + "\n";
    str += jv4sx.disp.getCamera().getViewDir().getEntry(2) + "\n";
    str += jv4sx.disp.getCamera().getPosition().getEntry(0) + "\n";
    str += jv4sx.disp.getCamera().getPosition().getEntry(1) + "\n";
    str += jv4sx.disp.getCamera().getPosition().getEntry(2) + "\n";
    str += jv4sx.disp.getCamera().getInterest().getEntry(0) + "\n";
    str += jv4sx.disp.getCamera().getInterest().getEntry(1) + "\n";
    str += jv4sx.disp.getCamera().getInterest().getEntry(2) + "\n";
    str += jv4sx.getScale() + "\n";
    str += jv4sx.scaleMin + "\n";
    str += jv4sx.scaleMax + "\n";
    str += bgColorButton.getBackground().getRed() + "\n";
    str += bgColorButton.getBackground().getGreen() + "\n";
    str += bgColorButton.getBackground().getBlue() + "\n";
    str += antialiasing.isSelected() + "\n";
    str += getClipMode() + "\n";
    str += getClipRadius() + "\n";

    str += eqAdm.save();
    str += cuAdm.save();
    str += parAdmin.save();
    str += solPtsAdm.save();
    return (str);
  }

    public void save(String filelocation) {
	if (surfex_.inAnApplet) {
	    try {
		// ??? this should be done for all open projects!!!
		URL url = new URL(surfex_.webPrgs.surfPath.getText()
				  + "surfex_comp.php");
		URLConnection connection = url.openConnection();
		connection.setUseCaches(false);
		connection.setDoOutput(true);
		
		PrintWriter out = new PrintWriter(connection.getOutputStream());
		//System.out.println("save
		// tmpdir:"+surfex_.getCurrentProject().old_tmpsubdir);
		out
		    .print("tmpsubdir="
			   + URLEncoder
			   .encode(this.old_tmpsubdir)
			   + "&prg=save" + "&filelocation="
			   + URLEncoder.encode(filelocation)
			   + "&prg_code_part1="
			   + URLEncoder.encode(getSuxFile()));
		out.close();
		
		BufferedReader in = new BufferedReader(new InputStreamReader(
							   connection.getInputStream()));
		String inputLine = in.readLine();
		if (inputLine.equals("okay")) {
		    // it worked
		    inputLine = in.readLine();
		    this.old_tmpsubdir = in.readLine();
		}
	    } catch (Exception e) {
		System.out.println("closeSurfex:" + e.toString());
	    }
	    	    
	    return;
	}
	
	// Project speichern
	// lege datei an oder ueberschreibe
	if (filename == null) {
	    System.out.println("null file");
	} else {
	    try {
		String proName = ((new File(filelocation)).getCanonicalPath()).substring
		    (((new File(filelocation)).getCanonicalPath()).lastIndexOf(File.separator)+1);
		projectName = proName;
		surfex_.pane.setTitleAt(surfex_.pane.getSelectedIndex(), projectName);
	    } catch(Exception ex) {
		System.out.println("Exception in loadProject:"+ex.toString());
	    }
	    //System.out.println("filenem");
	}
	try {
	    FileOutputStream fo = new FileOutputStream(filename);
	    /*
	     * JDK-1.0-Version:
	     * 
	     * PrintStream ps = new PrintStream(fo); ps.println(dStr);
	     */
	    PrintWriter pw = new PrintWriter(fo, true);
	    pw.println(getSuxFile());
	    pw.close();
	} catch (IOException er) {
	    System.out.println(er);
	}
    }
    
    // Ereignismethoden
    public void actionPerformed(ActionEvent e) {
	String command = e.getActionCommand();
	//  System.out.println(command );
	
	actionCommand(command);
  }

  public void actionCommand(String command) {

    if (command == "save") {
      if (filename == null) {
        command = "saveas";
      } else {
        save(filename);
      }
    }

    if (command == "saveas") {
      // creating new Frame
      final JFrame frame2 = new JFrame("save " + projectName + " as");

      // adding container
      Container contentPane = frame2.getContentPane();

      if (surfex_.inAnApplet) {
        ActionListener aL = new ActionListener() {
          public void actionPerformed(ActionEvent actionEvent) {
            String command2 = actionEvent.getActionCommand();
            //System.out.println("com:"+command2);
            // check if doubleclickt or if "open" was pressed
            if (command2.equals("  save  ")) {
              // creating complete filepath
              String filelocation = tf.getText();
              //System.out.println("file:"+filelocation);
              // ausgabe fehler bein z.B.: d:\ -> d:\\ bzw \ -> \\
              //          System.out.println("filelocation : "+
              // filelocation + "\n");
              save(filelocation);

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
        tf = new JTextField("test.sux");
        contentPane.add(tf, BorderLayout.CENTER);
        JPanel tmpPanel = new JPanel();
        tmpPanel.setLayout(new FlowLayout());
        //String strFormats[] = { "sux" };
        JButton jbsave = new JButton("  save  ");
        jbsave.addActionListener(aL);
        tmpPanel.add(jbsave);
        JButton jbcancel = new JButton("  cancel  ");
        jbcancel.addActionListener(aL);
        tmpPanel.add(jbcancel);
        contentPane.add(tmpPanel, BorderLayout.EAST);
        frame2.setSize(500, 75);
      } else {
        // creating Filechooser
        JFileChooser fileChooser = new JFileChooser(
            surfex_.currentDirectory);

        // adding filechooser
        contentPane.add(fileChooser, BorderLayout.CENTER);

        // Create ActionListener
        ActionListener actionListener = new ActionListener() {
          public void actionPerformed(ActionEvent actionEvent) {
            JFileChooser theFileChooser = (JFileChooser) actionEvent
                .getSource();

            // get command2
            String command2 = actionEvent.getActionCommand();

            // check if doubleclickt or if "open" was pressed
            if (command2.equals(JFileChooser.APPROVE_SELECTION)) {

              File selectedFile = theFileChooser
                  .getSelectedFile();

              // making savepopup invisible/removing it
              frame2.dispose();

              // creating complete filepath
              String filelocation = new String(selectedFile
                  .getParent()
                  + File.separator + selectedFile.getName());

              // ausgabe fehler bein z.B.: d:\ -> d:\\ bzw \ -> \\
              //          System.out.println("filelocation: "+
              // filelocation);

              //                   speicher den Kram unter filename;
              //                     filename=filelocation+"_"+projectName+".sux";
              if (filelocation.indexOf(".sux") == -1) {
                filename = filelocation + ".sux";
              } else {
                filename = filelocation;
              }
              save(filename);
            } else {
              //if(command.equals(JFileChooser.CANCEL_SELECTION)){
              frame2.setVisible(false);
              //}
            }
          }
        };

        FileFilter acfFilter = new ExtensionFileFilter(
            "all common files", new String[] { "sux" });
//        fileChooser.addChoosableFileFilter(acfFilter);

        FileFilter suxFilter = new ExtensionFileFilter(
            "*.sux (sux-file)", new String[] { "sux" });
        fileChooser.addChoosableFileFilter(suxFilter);
	fileChooser.setDialogType(JFileChooser.SAVE_DIALOG);
	String theName = "";
	try {
	    File lastFile = new File(filename);
	    theName = lastFile.getName();
	    fileChooser.setSelectedFile(lastFile);
//	    System.out.println("name:"+theName);
	} catch(Exception fileEx) {
	    System.out.println("fileEx:"+fileEx.toString());
	}


        // adding actionListener
        fileChooser.addActionListener(actionListener);
      }
      frame2.pack();
      frame2.setVisible(true);
    }

    if (command == "closeProject") {
      // schliesse dieses Projekt
      // popup with questions, if you want to save before leavin
      Object[] butts = { "Ja", "Nein", "Abbrechen" };
      String initbutt = "Ja";
      int optReturn = JOptionPane.showConfirmDialog(new Panel(),
          "Do you want to save \"" + projectName
              + "\" before leaving?\n", " close?",
          JOptionPane.YES_NO_CANCEL_OPTION,
          JOptionPane.QUESTION_MESSAGE);
      //System.out.println(optReturn);
      switch (optReturn) {
      case 0:
        // ja, speichern
        save_as_and_close_Project();
        break;
      case 1:
        //nein , ggf. sicherungskopie anlegen
        // Projekt schliessen
        closeProject();
        break;
      case 2: // abbrechen, also nic machen ...
        break;
      }
    }

    if (command == "savePic") {
      // tollen Bildspeicherdialog anzeigen ....
      showSavePicDialog();
    }
    
    if (command == "saveMovie") {
      // tollen Bildspeicherdialog anzeigen ....
      showSaveMovieDialog();
    }
    
    if(command =="configure lamps"){
    	lampAdmin.showall();
    }
  }

  public void saveFile(String filelocation, int savetype, boolean dithered,
      boolean antialiasing, int h, int w, int d,jv4surfex jv4sx) {
    //System.out.println("saveFile..."+antialiasing+",
    // savetype:"+savetype);
    if (savetype == 0) {
	eqAdm.saveSurfCode(filelocation, filelocation + ".jpg",
			   antialiasing,jv4sx);
    }
    if (savetype == 1) {
/*
  eqAdm.savePovCode(filelocation);
*/
    }
    if (savetype == 2) {
	eqAdm.saveSurfCode(filelocation, filelocation + ".jpg",
			   antialiasing,jv4sx);
    }
    if (savetype == 3) {
	SavePic saveThread = new SavePic(filelocation, dithered,
					 antialiasing, h, w, d, surfex_, this,this.jv4sx.getCamPos(),
					 this.jv4sx.getViewDir(),this.jv4sx.getUpVector(),this.jv4sx.getRightVector(),
					 parAdmin.getAllParams(),parAdmin.getAllParamValues(),
					 this.jv4sx,this.getAllLamps());  
	
	saveThread.start();
    }
  } // end of saveFile()
  
  
    
    public void loadProject(String filelocation) {
	try {
	    filename = filelocation;
	    String proName = ((new File(filelocation)).getCanonicalPath()).substring
		(((new File(filelocation)).getCanonicalPath()).lastIndexOf(File.separator)+1);
	    projectName = proName;
	    surfex_.pane.setTitleAt(surfex_.pane.getSelectedIndex(), projectName);
	} catch(Exception ex) {
	    System.out.println("Exception in loadProject:"+ex.toString());
	}
//	System.out.println("file:"+filelocation);
	File fi = new File(filelocation);
	try {
	    FileInputStream fs = new FileInputStream(fi);
//	    System.out.println("input.");
	    BufferedReader bs = new BufferedReader(new InputStreamReader(fs));
//	    System.out.println("stream");
//	    System.out.println("loadProject bs...");
	    loadProject(bs);
//	    System.out.println("loaded.");
	} catch (Exception e) {
	    System.out.println(e.toString());
	}
    }
    
    public void loadProject(URL url) {
	try {
	    BufferedReader bs = new BufferedReader(
		new InputStreamReader(url.openStream()));
	    loadProject(bs);
	} catch (Exception e) {
	    System.out.println(e.toString());
	}
    }
    
    public void loadProject(BufferedReader bs) {
	try {
	    String version = bs.readLine().substring(16);
	    String version_num = version.substring(0, version.indexOf(".")) 
		+ version.substring(version.indexOf(".")+1, version.indexOf(".")+3)
		+ version.substring(version.indexOf(".")+4);
	    int iVersion = Integer.parseInt(version_num);
	    
//	    System.out.println("ver:"+iVersion);
	    String nix = bs.readLine();
	    String datatype = bs.readLine(); // one out of: "complete", "specify", 
	    // "onlyeqns", "onlystyle", "eqnsvis"
	    //System.out.println("datatype:"+datatype);
	    if (datatype.equals("specify")) {
		try {
		    String whichinfo;
		    bs.mark(255);
		    whichinfo = bs.readLine();
		    //System.out.println("whichinfo:"+whichinfo);
		    while (whichinfo.equals("GENERAL DATA:")
			   || whichinfo.equals("EQUATIONS:")
			   || whichinfo.equals("CURVES:")
			   || whichinfo.equals("PARAMETERS:")
			   || whichinfo.equals("SOLITARY POINTS:")) {
			bs.reset();
//			System.out.println("general data");
			if (whichinfo.equals("GENERAL DATA:")) {
			    eqAdm.loadGeneralData(bs, datatype, iVersion);
			}
//			System.out.println("equations");
			if (whichinfo.equals("EQUATIONS:")) {
			    eqAdm.loadEquations(bs, datatype, iVersion);
			}
//			System.out.println("curves");
			if (whichinfo.equals("CURVES:")) {
			    cuAdm.loadCurves(bs, datatype, iVersion);
			}
//			System.out.println("parameters");
			if (whichinfo.equals("PARAMETERS:")) {
			    parAdmin.loadParameters(bs, datatype, iVersion);
			}
//			System.out.println("solitary points");
			if (whichinfo.equals("SOLITARY POINTS:")) {
			    solPtsAdm.loadSolitaryPoints(bs, datatype, iVersion);
			}
			bs.mark(255);
			whichinfo = bs.readLine();
			//System.out.println("whichinfo:"+whichinfo);
		    }
		} catch (Exception ex) {
		    bs.reset();
		}
	    } else {
		// i.e. "complete":
		eqAdm.loadGeneralData(bs, datatype, iVersion);
		eqAdm.loadEquations(bs, datatype, iVersion);
		cuAdm.loadCurves(bs, datatype, iVersion);
		parAdmin.loadParameters(bs, datatype, iVersion);
		solPtsAdm.loadSolitaryPoints(bs, datatype, iVersion);
	    } // end of else
	    
	} catch (IOException e) {
	    System.out.println("loadProject-Error:" + e);
	}
    }
    
    public void scaleSliderUpdated() {
	//System.out.println("ssUpd:"+Double.parseDouble(scaleLabel.getText()));
	//  eqAdm.lastScale = Double.parseDouble(scaleLabel.getText());
	//System.out.println("scale:"+scaleLabel.getText());
//    System.out.println("setscale:"+eqAdm.lastScale);
//    eqAdm.jv4sx.disp.getCamera().setScale(eqAdm.lastScale);
	jv4sx.setScale(eqAdm.lastScale);
	eqAdm.jv4sx.updateDisp();
	//surfex_.raytrace();
	//eqAdm.updateJV4SXandReconstructLastView();
	
    }
} // end class Project

