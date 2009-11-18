////////////////////////////////////////////////////////////////////////
//
// This file surfex.java is part of SURFEX.
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

/////////////////////////////////////////////////////////////////////////////////
//
//  surfex is a java-program for visualizing real algebraic geometry.
//
//  Actually, surfex does not produce any visualization by itself,
//  but surfex uses external tools to do that.
//
//  Unfortunately, surfex started as a quick hack during a Dagstuhl workshop in 2001, 
//  was then extended step by step, and was never redesigned from scratch. 
//  This causes surfex not to be a perfect piece of software.
//  Nevertheless, we hope that surfex will be useful for someone.
//
//  September 2008, The authors.
//
//  -----------------------------------------------------------------------------
//
//  Several external visualization tools we use (or maybe will use in the future):
//  - raytracing: 
//    - surf, 
//    - povray (currently not used!)
//    - hopefully, in the future, some GPU-raytracers...
//  - triangulation: 
//    - impsurf/asurf (currently not used!) 
//    - hopefully, in the future, some 
//  - display of triangulated data: 
//    - JavaView
//
//  Main Features:
//  - high quality output for printed publications and for the internet
//  - interactivity, in particular: intuitive rotation/scaling
//  
//  @version 0.90.00, 2008 / 9 
//  @author Stephan Holzer, Oliver Labs
//          idea: Oliver Labs (2001) during the Dagstuhl workshop 
//                Algebra, Geometry, and Software Systems
//          this version is based on some 
//                older code by Oliver Labs and also some ideas by Richard Morris
//  @address department of mathematics, Saarland University at Saarbruecken, Germany
//
//  In addition to JavaView and our code, we use the following java-file:
//  - jv4surfex.java (implemented by the JavaView authors, for the interface to JavaView)
// 
//  to do: 
//  - bug-fixes
//  - make the command line version a real command line version 
//    (i.e. do not require frames internally)
//  - comfortable installation program/documentation for unix/mac/windows/...
//  - improve documentation for end users
//  - improve documentation for developers (e.g.: us!) who want to extend/bug-fix surfex
//  - improve documentation for developers who want to use it (API)
//    - write Singular/Maple/... interfaces for surfex using this API
//  - there are many interesting features to be implemented such as:
//    - finish the feature which allows users to place lamps interactively 
//      (Lamp.java and LampAdmin.java)
//    - ...
//
//////////////////////////////////////////////////////////// //

////////////////////////// /
//  we need some classes from the libraries:
//  (this can be optimized!!!)
//

import java.applet.AppletContext;
import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import java.util.*;
import javax.swing.filechooser.*;
import javax.swing.filechooser.FileFilter;
import javax.swing.colorchooser.*;
import javax.swing.ImageIcon;
import java.io.*;
import java.awt.image.*;
import java.net.*;
import jv.vecmath.PdVector;
import java.util.Random;

//////////////////////////////////////////////////////////// //
//
//  class surfex 
//
//////////////////////////////////////////////////////////// //
class surfex extends JFrame implements ActionListener {
    // Anfang Variablen
    static String strVersion = "0.90.00";
    
    static String strDate = "September 2008";
    
    public static String tmpDir = "/tmp/";
    //   public static String tmpDir = "";
    
    public static String defaultBrowser = "konqueror";
    
    public final double cm_inch_ratio=2.4;

    public boolean initFlag = true;
    
    boolean inAnApplet = false;
    
    Applet theApplet;
    
    boolean iaa;
    
    public boolean noWindow = false;
    
    boolean theOtherRayUpdateIsReady=true;
    boolean lampRayUpdateIsReady=true;
    
    
    public String filePrefix = "";
    
    public boolean showMoviePreview=false;
    
    public String currentDirectory = ".";

    public String surfexDirectory = ".";
    
    Thread t=new Thread();
    
    
    public String fileFormat = "";
    
    public String outputFilename = "";
    
    public boolean quitAfterwards = false;
    
    public LampAdmin lampAdmin;
    
    // GUI
    JMenuBar mb = new JMenuBar();
    
    surfex surfex_;
    
    static JFrame pFrame = new JFrame("Starting surfex v. "+strVersion+"...");

    
    static SurfexStartFrame  progressFrame=new SurfexStartFrame(1000); // damit man angezeigt bekommt, wie lang er noch zum starten braucht
    
    //Applet theApplet;
    
    UpdateRayframeImmediatlyThread updateRayframeImmediatlyThread;
    
    JTabbedPane pane = new JTabbedPane();
    
    RayFrame rayFrame;
    
    // Projects
    Vector openProjects = new Vector();
    
    int projectCounter = 0;
    
    ConfigFrame configFrame;// = new ConfigFrame();
    
    ConfigFrame webPrgs;
    
    int osLINUX = 0;
    
    int osWINDOWS = 1;
    
    int OS;//= osWINDOWS;
    //    int OS = osLINUX;
    
    public static int cygwin = 0; // if osLINUX, but actually cygwin
    public static String cygwindir = ""; // if osLINUX, but actually cygwin
    public static String cygdrivedir = "/cygdrive/"; // if osLINUX, but actually cygwin
    
    Project lastProject; // das Projekt, das vor dem current zuletzt geoeffnet wurde
    
    // etc
    
    public static jv4surfex jv4sx = null;
    
    // Ende Variablen

    public static String changeFilenameExtension(String filename, String newExt) {
	String newFilename = filename;
	int i = newFilename.lastIndexOf(".");
	if(i>0) {
	    newFilename = newFilename.substring(0,i) + newExt;
	} else {
	    newFilename = newFilename.substring(0,i) + newExt;
	}
	return(newFilename);
    }

    public int proceedAlert() {
	Object[] options = {"Yes",
			    "No"};
	int n = JOptionPane.showOptionDialog(this,
					     "If you proceed then all current data will be lost. Do you want to proceed?",
					     "Data will be lost - proceed?",
					     JOptionPane.YES_NO_OPTION,
					     JOptionPane.QUESTION_MESSAGE,
					     null,
					     options,
					     options[0]);
	return(n);
    }

    public int clearCurProj() {
	Project pro = getCurrentProject();

	int n = proceedAlert();	
	if(n==0) {
	    pro.clear();
	    pro.projectName = "pro "+pro.ProjectNumber;
	    pane.setTitleAt(pane.getSelectedIndex(), pro.projectName);
	    pane.repaint();
	}
	return(n);
    }
	
    public void actionPerformed(ActionEvent e) {
	String command = e.getActionCommand();
	//  System.out.println(command );
	if (command == "clear") {
	    clearCurProj();
	}
	if (command == "new") {
	    Project pro = newProject();
//	    System.out.println("project created.");
//	    System.out.println("project scale:"+pro.getScale());
// 	    System.out.println("old index:"+(pane.getTabCount()-1));
 	    pane.add(pro, "pro " + projectCounter);
// 	    System.out.println("index:"+pane.getSelectedIndex());
// 	    System.out.println("new index:"+(pane.getTabCount()-1));
 	    pane.setSelectedIndex(pane.getTabCount()-1);
	    pane.repaint();
// 	    System.out.println("index:"+pane.getSelectedIndex());
//	    pro.newCurve();
//	    pro.newEquation();
	    //pro.newCurve();
	    //break;
	}
	if(command == "helpfiles") {
	    if(inAnApplet) {
		try {
		    surfex_.theApplet.getAppletContext().showDocument(
			new URL(surfex_.theApplet.getCodeBase() + "help/"),
			"_blank");
		} catch (Exception eh) {
		    System.out.println(eh.toString());
		}
	    } else {
		// open the documentation PDF
		try {
		    Runtime r = Runtime.getRuntime();
		    Process p;
		    if(surfex_.cygwin==1) {
			p = r.exec(defaultBrowser + " " + surfexDirectory + File.separator + 
				   "doc" + File.separator + "surfex_doc_linux.pdf");
		    } else {
//			System.out.println("cb:"+util.getCodebase(this.getClass()));
			p = r.exec(defaultBrowser + " " + surfexDirectory + File.separator + 
				   "doc" + File.separator + "surfex_doc_linux.pdf");
		    }
		} catch(Exception eh) {
		}
	    }
	}
	if(command == "surfexWWW") {
	    if(inAnApplet) {
		try {
		    surfex_.theApplet.getAppletContext().showDocument(
			new URL("http://www.surfex.algebraicsurface.net"),
			"_blank");
		} catch (Exception eh) {
		    System.out.println(eh.toString());
		}
	    } else {
		// open the surfex website in a browser
		try {
		    Runtime r = Runtime.getRuntime();
		    Process p;
		    if(surfex_.cygwin==1) {
			p = r.exec(defaultBrowser + " http://www.surfex.algebraicsurface.net");
		    } else {
//			System.out.println("cb:"+util.getCodebase(this.getClass()));
			p = r.exec(defaultBrowser + " http://www.surfex.algebraicsurface.net");
		    }
		} catch(Exception eh) {
		}
	    }
	}
	if (command == "about") {
	    AboutFrame a = new AboutFrame(this);
	}
	if (command == "configuration") {
	    configFrame.setVisible(true);
	}
	if (command == "load") {
	    // creating new Frame
	    final JFrame frame2 = new JFrame("load");
	    
	    // adding container
	    Container contentPane = frame2.getContentPane();
	    
	    // creating Filechooser
	    JFileChooser fileChooser = new JFileChooser(surfex_.currentDirectory);
	    
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
//			    System.out.println("file selected to open");
			    File selectedFile = theFileChooser.getSelectedFile();
			    currentDirectory = theFileChooser.getCurrentDirectory().getAbsolutePath();
			    
			    // making savepopup invisible/removing it
//			    System.out.println("dispose...");
			    frame2.dispose();
			    
			    // creating complete filepath
			    String filelocation = selectedFile.getAbsolutePath();
			    
			    // ausgabe fehler bein z.B.: d:\ -> d:\\ bzw \ -> \\
//			    System.out.println("filelocation : "+ filelocation + "\n");
//			    System.out.println("load..."+filelocation);	
			    load(filelocation);
//			    System.out.println("loaded.");			    
			} else {
			    //if(command.equals(JFileChooser.CANCEL_SELECTION)){
			    frame2.setVisible(false);
			    //}
			}
		    }
		};
	    
	    FileFilter acfFilter = new ExtensionFileFilter("all common files",
							   new String[] { "sux" });
//	    fileChooser.addChoosableFileFilter(acfFilter);
	    
	    FileFilter suxFilter = new ExtensionFileFilter("*.sux (sux-file)",
							   new String[] { "sux" });
	    fileChooser.addChoosableFileFilter(suxFilter);
	    fileChooser.setDialogType(JFileChooser.OPEN_DIALOG);
	    
	    // adding actionListener
	    fileChooser.addActionListener(actionListener);
	    frame2.pack();
	    frame2.setVisible(true);
	    
	    //oeffen den Kram unter filename;
	    
	} // end: command == "load"
	if (command == "saveas" || command == "savePic" || command == "saveMovie"
	    || command == "closeProject") {
	    getCurrentProject().actionCommand(command);
	}
	if(command == "quitSurfex") {
	    // better try to close all projects first!!!
	    if(proceedAlert()==0) {
		// then quit:
		System.exit(0);
	    }
	}
    }
    
    public Project getCurrentProject() {
	int pi = pane.getSelectedIndex();
	if(pi!=-1){
	    return ((Project) openProjects.get(pi));
	}
	return null;
    }
    
    public JMenuBar createMenuBar() {
	
   	mb = new JMenuBar();
	
	JMenu file = new JMenu("file");

	JMenuItem miclearProject = new JMenuItem("clear current project");
	miclearProject.addActionListener(this);
	miclearProject.setActionCommand("clear");
	if (inAnApplet) {
	    miclearProject.setEnabled(false);
	} else {
//	    miclearProject.setEnabled(false);
	}
	
	JMenuItem minewProject = new JMenuItem("new project");
	minewProject.addActionListener(this);
	minewProject.setActionCommand("new");
	if (inAnApplet) {
	    minewProject.setEnabled(false);
	} else {
//	    minewProject.setEnabled(false);
	}
	
	JMenuItem load = new JMenuItem("load project...");
	load.addActionListener(this);
	load.setActionCommand("load");
	if (inAnApplet) {
	    load.setEnabled(false);
	} else {
//	    load.setEnabled(false);
	}
	
	JMenuItem save = new JMenuItem("save this project...");
	save.addActionListener(this);
	save.setActionCommand("saveas");
	
	JMenuItem savePic = new JMenuItem("save this raytraced picture as...");
	savePic.addActionListener(this);
	savePic.setActionCommand("savePic");
	
	JMenuItem saveMovie = new JMenuItem("create and save a movie of this surface as...");
	saveMovie.addActionListener(this);
	saveMovie.setActionCommand("saveMovie");
	
	JMenuItem closeProject = new JMenuItem("close this project");
	closeProject.addActionListener(this);
	closeProject.setActionCommand("closeProject");
	closeProject.setEnabled(false);
	
	JMenuItem quitSurfex = new JMenuItem("quit surfex");
	quitSurfex.addActionListener(this);
	quitSurfex.setActionCommand("quitSurfex");
		
	file.add(miclearProject);
//	file.add(minewProject);
	file.add(load);
	file.add(save);
	file.add(savePic);
	file.add(saveMovie);
	file.add(closeProject);
	file.add(quitSurfex);
	
	mb.add(file);
	
	JMenu extras = new JMenu("extras");
	JMenuItem configuration = new JMenuItem("configuration");
	configuration.addActionListener(this);
	configuration.setActionCommand("configuration");
	configuration.setEnabled(false);
	
	extras.add(configuration);
	
	mb.add(extras);
	
	JMenu help = new JMenu("help");
	JMenuItem helpfiles = new JMenuItem("help files");
	helpfiles.addActionListener(this);
	helpfiles.setActionCommand("helpfiles");
//	helpfiles.setEnabled(false);

	JMenuItem surfexWWW = new JMenuItem("surfex on the web");
	surfexWWW.addActionListener(this);
	surfexWWW.setActionCommand("surfexWWW");
	
	JMenuItem about = new JMenuItem("about");
	about.addActionListener(this);
	about.setActionCommand("about");
	
	help.add(helpfiles);
	help.add(surfexWWW);
	help.add(about);
	
	mb.add(help);
	
	return mb;
    }
    
    public void load(String filelocation) {
//	System.out.println("load 1:"+filelocation);
	if(cygwin==1) {
	    filelocation = toWindows(filelocation);
	}
	Project project;
	try {
//	    String proName = ((new File(filelocation)).getCanonicalPath()).substring
//		(((new File(filelocation)).getCanonicalPath()).lastIndexOf(File.separator)+1);
//	    System.out.println("proName:"+proName);

//  	    project  = new Project(this, jv4sx, ++projectCounter,
//  				   proName);
// 	    System.out.println("loadProject...");
// 	    project.loadProject(filelocation);
// 	    openProjects.add(project);
// 	    pane.add(project, proName);
// 	    pane.setSelectedIndex(pane.getTabCount() - 1);
// 	    System.out.println("tabChanged...");
// 	    tabChanged();
// 	    System.out.println("raytrace...");

//	    System.out.println("load project...");
	    if(initFlag) {
		project = getCurrentProject();	    
		project.loadProject(filelocation);
	    } else {
		if(clearCurProj()==0) {
		    project = getCurrentProject();	    
		    project.loadProject(filelocation);
		}
	    }
	    
//	    tabChanged();

	    surfex_.raytrace();
	} catch(Exception ex) {
	}
	// datei einlesen und project aufbauen
	
    }
    
    public void load(BufferedReader bs) {
//	System.out.println("load 2");
	Project project = new Project(this, jv4sx, ++projectCounter, "");
	// datei einlesen und project aufbauen

	project.loadProject(bs);
	openProjects.add(project);
	pane.add(project, project.projectName);
	pane.setSelectedIndex(pane.getTabCount() - 1);
	tabChanged();
    }
    
    public void raytrace() {
	getCurrentProject().bt2ActionPerformed(null);
    }
    
    public Project newProject() {
//	System.out.println("nP");	
	Project project = new Project(this, jv4sx, ++projectCounter, "");

//	System.out.println("add...");	
	openProjects.add(project);
//	System.out.println("new project added:"+project.ProjectNumber);
	//JScrollPane mainpane = new JScrollPane(project);
	return project;
    }
    
    public void closeProject(int ProjectNumber) {
	if (openProjects.size() >= 2) {
	    ListIterator li = openProjects.listIterator();
	    int i = 1;
	    while (li.hasNext()) {
		if (((Project) li.next()).ProjectNumber == ProjectNumber) {
		    openProjects.removeElementAt(i);
		    break;
		}
	    }
	} else {
	    openProjects.clear();
	    projectCounter = 0;
	}
	updateProjectPane();
    }
    
    public void updateProjectPane() {
	pane.removeAll();
	ListIterator li = openProjects.listIterator();
	Project pro;
	while (li.hasNext()) {
	    pro = (Project) li.next();
	    JScrollPane mainpane = new JScrollPane(pro);
	    pane.add(mainpane, pro.projectName);
	}
	SwingUtilities.updateComponentTreeUI(this);
    }
    
    public void closeSurfex() {
	//       System.out.println("closeSurfex()");       
	if (pane.getSelectedIndex() >= 0) {
	    //	   System.out.println(">=0 !");
	    if(inAnApplet) {		
		// stop the raytracer-frame, if it runs:
		try {
		    updateRayframeImmediatlyThread.stop();
		} catch (Exception ex) {
		    //        System.out.println("stop thread:"+ex.toString());
		}
		
		// close all the windows:
		System.out.println("close all windows...");
		configFrame.setVisible(false);
		rayFrame.setVisible(false);
		jv4sx.hide();
		setVisible(false);
		
		// remove the temporary directory:
		try {
		    // ??? this should be done for all open projects!!!
		    URL url = new URL(surfex_.webPrgs.surfPath.getText()
				      + "surfex_comp.php");
		    URLConnection connection = url.openConnection();
		    connection.setUseCaches(false);
		    connection.setDoOutput(true);
		    
		    PrintWriter out = new PrintWriter(connection
						      .getOutputStream());
		    //        System.out.println("remove tmpdir:"+surfex_.getCurrentProject().old_tmpsubdir);
		    out.print("tmpsubdir="
			      + URLEncoder
			      .encode(getCurrentProject().old_tmpsubdir)
			      + "&prg=rmtmp");
		    out.close();
		    
		    BufferedReader in = new BufferedReader(
							   new InputStreamReader(connection.getInputStream()));
		    String inputLine = in.readLine();
		    if (inputLine.equals("okay")) {
			// it worked
		    }
		} catch (Exception e) {
		    System.out.println("closeSurfex:" + e.toString());
		}
		getCurrentProject().old_tmpsubdir = "";
		
	    } else {
		//	       System.out.println("not in applet");
		double ang[] = getCurrentProject().eqAdm.getAngles();
		double scale[] = getCurrentProject().eqAdm.getScales();
		//System.out.println("scale_x=" + scale[0] + "; scale_y="
		//   + scale[1] + "; scale_z=" + scale[2] + "; rot_y="
		//  + ang[0] + "; rot_x=" + ang[1] + "; rot_z=" + ang[2]
		// + ";");
		System.out.println("close?");
		if(proceedAlert()==0) {
		    System.exit(0);
		}
		//	       updateRayframeImmediatlyThread.stop();
		//	       System.exit(0);
	    }
	} else {
	    if(proceedAlert()==0) {
		System.exit(0);
	    }
//	    System.out.println("ind:"+pane.getSelectedIndex());
	}
    }
    
    public surfex(String title, Applet theApplet, boolean iaa, String cb,
		  String tempdir, 
		  String cygdrive, 
		  boolean noWindow) {
	// Frame-Initialisierung
	super(title);
	this.noWindow=noWindow;
	
	initFlag = false;

	surfexDirectory = (new File(".")).getAbsolutePath();
	
	//     tmpDir = tempdir;
	if(tempdir.length()>0) {
	    tmpDir = tempdir;
	}
	
	//
	if(cygdrive.length()>0) {
	    cygdrivedir = cygdrive;
	}
	
	//
	this.theApplet=theApplet;
	this.iaa=iaa;
	
	progressFrame.refresh(55,"triangulated preview...");
	if (iaa) {
	    jv4sx = new jv4surfex(true, theApplet, this,new JTextField(),
				  new JTextField(),new JTextField());
	    this.theApplet = theApplet;
	} else {
	    jv4sx = new jv4surfex(false, null, this,
				  new JTextField(),
				  new JTextField(),
				  new JTextField());
	}
	//     System.out.println("s1 "+jv4sx.getScale());
	
	inAnApplet = iaa;
	
	filePrefix = cb;
	
	//     System.out.println("filesep:"+File.separator);
	if ((File.separator).equals("/")) {
	    OS = osLINUX;
	} else {
	    OS = osLINUX;
	    cygwin = 1;

	    // cygwin directories: 
	    try {
		Runtime r = Runtime.getRuntime();
		Process p;
		p = r.exec("sh -exec mount");

		InputStreamReader isr = new InputStreamReader(p.getInputStream());
		BufferedReader in = new BufferedReader(isr);
		String inputLine;
		String str;
		str = "";
		int ind = 0;
		while ((inputLine = in.readLine()) != null) {
		    str += inputLine + "\n";
		    ind = inputLine.indexOf(" on / ");
		    if(ind >= 0) {
//			System.out.println("i:"+inputLine.substring(0,ind)+".");
			cygwindir = inputLine.substring(0,ind);
		    }
// 		    ind = inputLine.indexOf(" on /cygdrive/");
// 		    if(ind >= 0) {
// 			System.out.println("i:"+inputLine.substring(0,ind)+".");
// 			cygdrivedir = inputLine.substring(0,ind);
// 		    }
		    ////System.out.println(inputLine);
		} // end while(in.readLine())
		in.close();
//		System.out.println("mount:"+str+"\n"+".");
		p.waitFor();
		
	    } catch(Exception e) {
		System.out.println("ERROR cygwindrive: "+e);
	    }
	} 
	OS=osLINUX;
	
	surfex_ = this;
	
	if (OS == osWINDOWS) {
	    configFrame = new ConfigFrame(
		"C:\\Programme\\POV-Ray for Windows v3.6\\bin\\pvengine",
		"",
		"",
		"iview\\i_view32.exe",
		2,
		"http://enriques.mathematik.uni-mainz.de/labs/algebraicsurface/surfex/surfex_Path.txt",
		updateRayframeImmediatlyThread, this);
	}
	if (OS == osLINUX) {
	    if(cygwin==1) {
		configFrame = new ConfigFrame(
		    "povray",
		    "surf -n ",
		    "./localimpsurfCV",
		    "",
		    2,
		    "http://enriques.mathematik.uni-mainz.de/labs/algebraicsurface/surfex/surfex_Path.txt",
		    updateRayframeImmediatlyThread, this);
	    } else {
		configFrame = new ConfigFrame(
		    "povray",
		    "surf -n ",
		    "./localimpsurfCV",
		    "",
		    2,
		    "http://enriques.mathematik.uni-mainz.de/labs/algebraicsurface/surfex/surfex_Path.txt",
		    updateRayframeImmediatlyThread, this);
	    }
	}
	if (inAnApplet) {
	    webPrgs = new ConfigFrame("", filePrefix, filePrefix, filePrefix,
				      2, filePrefix + "surfex_paths.txt",
				      updateRayframeImmediatlyThread, this);
	} else {
	    // non-applets can decide which server should be used as a compute server:
	    if (1 == 1) {
		webPrgs = new ConfigFrame(
		    "",
		    "http://127.0.0.1/oliverlabs.net/algebraicsurface/surfex/",
		    "http://127.0.0.1/oliverlabs.net/algebraicsurface/surfex/",
		    "http://127.0.0.1/oliverlabs.net/algebraicsurface/surfex/",
		    2,
		    "http://127.0.0.1/oliverlabs.net/algebraicsurface/surfex/surfex_Path.txt",
		    updateRayframeImmediatlyThread, this);
	    } else {
		webPrgs = new ConfigFrame(
		    "",
		    "http://enriques.mathematik.uni-mainz.de/labs/surfex/",
		    "http://enriques.mathematik.uni-mainz.de/labs/surfex/",
		    "http://enriques.mathematik.uni-mainz.de/labs/surfex/",
		    2,
		    "http://enriques.mathematik.uni-mainz.de/labs/surfex/surfex_paths.txt",
		    updateRayframeImmediatlyThread, this);
	    }
	}
	// zum speichern der letzten Javaview einstellung, falls der tab gewechselt wird.
	
	
	progressFrame.refresh(100,"raytaced preview...");
	//   rayFrame = new RayFrame("raytraced surface",this.getCurrentProject());
	// if(!noWindow){
	
	rayFrame = new RayFrame("raytraced surface");
	rayFrame.setSize(200 + rayFrame.getInsets().left
			 + rayFrame.getInsets().right, 200 + rayFrame.getInsets().top
			 + rayFrame.getInsets().bottom);
	rayFrame.setLocation(824, 200);
	if(!noWindow){
	    rayFrame.setVisible(true);
	}
	rayFrame.addWindowListener(new WindowAdapter() {
		
		
		public void windowClosingEvent(WindowEvent we) {
		    closeSurfex();
		}
	    });
	//}
	if(noWindow){
	    rayFrame.setVisible(false);
	}
	this.addWindowListener(new WindowAdapter() {
		public void windowClosing(WindowEvent evt) {
//		    System.out.println("windowClosing");
		    System.exit(0);
//		    closeSurfex();
		}
	    });
	
	//     System.out.println("s2 "+jv4sx.getScale());
	
	progressFrame.refresh(200,"main window...");
	setBounds(0, 0, 824, 550);
	Container cp = getContentPane();
	cp.setLayout(new BorderLayout());
	
	progressFrame.refresh(300,"menu bar...");
	setJMenuBar(createMenuBar());
	
	cp.add(pane, BorderLayout.CENTER);
	
	progressFrame.refresh(400,"project...");
	//     System.out.println("s3a "+jv4sx.getScale());
	pane.add(newProject(), "pro " + projectCounter);
	//     System.out.println("s3b "+jv4sx.getScale());
	pane.addChangeListener(new ChangeListener() {
		public void stateChanged(ChangeEvent ev) {
		    tabChanged();
		}
	    });
	
	//  setVisible(true);
	
	//     System.out.println("s3bb "+jv4sx.getScale());
	progressFrame.refresh(600,"config frame...");
	//  if(!noWindow){
	//     System.out.println("s3c "+jv4sx.getScale());
	updateRayframeImmediatlyThread = new UpdateRayframeImmediatlyThread(
	    rayFrame, this, (Project) openProjects.lastElement(),jv4sx);
	//     System.out.println("s3d "+jv4sx.getScale());
	lastProject = (Project) openProjects.lastElement();
	progressFrame.refresh(1000, "started.");

	//     progressFrame.refresh(900,"        - lamp Admin");
	//     }
	
	//       System.out.println("s4 "+jv4sx.getScale());
	
	//     pFrame.setAlwaysOnTop(false);
	pFrame.setVisible(false);
	//     progressFrame.setVisible(false);//.refresh(900,"   - starting threads");
	pFrame=null;  

	initFlag = false;
	//lampAdmin.lampManagerToFront();
    } // end of surfex(...)
    
    public void showall() {	
   	if(!noWindow){
	    //        System.out.println("s43 "+jv4sx.getScale());
	    
	    jv4sx.show();
	    //        System.out.println("s48 "+jv4sx.getScale());
	    
	    rayFrame.setVisible(true);
	    rayFrame.toFront();
	    setVisible(true);
	    toFront();
   	} else {
	    setVisible(false);
	    rayFrame.setVisible(false);
	    jv4sx.setVisible(false);
	    
   	}
	//    System.out.println("s5 "+jv4sx.getScale());
	
	//  this.getCurrentProject().lampAdmin.lampManagerToFront();
    }
    
    public static String toWindows(String str) {
	//	System.out.println("str1:"+str);
	//	System.out.println("str2:"+str.replaceAll("/","\\\\"));
	String s;
	if(str.substring(0,1).equals("/")) {
	    s = str.replaceAll("/","\\\\");
	    s = cygwindir + s;
	} else {
	    s = str.replaceAll("/","\\\\");
	}
	//	System.out.println("s2:"+s);
	return(s);
    }
    
    public static String toLinux(String str) {
	//	System.out.println("S1:"+str);
	String s = str.replaceAll("\\\\","/");
	//	System.out.println("S2:"+s);
	int iColon = s.indexOf(":");
	if(iColon>=0) {
	    s = cygdrivedir + s.substring(0,iColon) + s.substring(iColon+1);
	}
	//	System.out.println("S3:"+s);
	return(s);	
    }
    
    public void tabChanged() {
	// get current tab um die triangulierung zu wechseln
	System.out.println("tabChanged()");

	surfex_.configFrame.last[4] = false;
	surfex_.updateRayframeImmediatlyThread.stop();
//     surfex_.updateRayframeImmediatlyThread.t=t;
	
	lastProject.eqAdm.lastUpVector = PdVector.copyNew(jv4sx.disp
							  .getCamera().getUpVector());
	lastProject.eqAdm.lastViewDir = PdVector.copyNew(jv4sx.disp.getCamera()
							 .getViewDir());
	lastProject.eqAdm.lastPosition = PdVector.copyNew(jv4sx.disp
							  .getCamera().getPosition());
	lastProject.eqAdm.lastInterest = PdVector.copyNew(jv4sx.disp
							  .getCamera().getInterest());
	lastProject.eqAdm.lastScale = jv4sx.getScale();
	//        //System.out.println( );
	
// 	System.out.println("tab change " +getCurrentProject().projectName );
	if (pane.getSelectedIndex() >= 0) {
	    System.out.println("tabChanged()... update java view" );
	    getCurrentProject().eqAdm.updateJV4SXandReconstructLastView();
	    
	    if (getCurrentProject().raytraceAlways.isSelected()
		&& !surfex_.configFrame.last[4]
		&& surfex_.pane.getSelectedIndex() >= 0) {
		getCurrentProject().startRaytraceAlways();
	    }
	    
	    if (configFrame.rayImme.isSelected()) {
		//      //System.out.println("rayImme" );
		double ang[] = getCurrentProject().eqAdm.getAngles();
		double scale[] = getCurrentProject().eqAdm.getScales();
		
		//    updateRayframeImmediatlyThread.stop();
		//    newUpdateRayframeImmediatlyThread();
		//    updateRayframeImmediatlyThread.start();
		
	    }
	    
	    lastProject = getCurrentProject();
	}
    }
    
    public void newUpdateRayframeImmediatlyThread() {
	updateRayframeImmediatlyThread.stop();
	updateRayframeImmediatlyThread.t=new Thread();
//     System.out.println(updateRayframeImmediatlyThread.rand+":::::::::::::::::::::::::::::");
	updateRayframeImmediatlyThread = new UpdateRayframeImmediatlyThread(
	    rayFrame, this, getCurrentProject(),jv4sx);
    }
    
    public static void showHelp() {
	System.out.println();
	System.out.println("surfex v. " + strVersion + " " + strDate);
	System.out.println();
	System.out.println("Stephan Holzer and Oliver Labs");
	System.out.println("Univ. of Mainz (Germany) and Univ. Saarbruecken (Germany)");
	System.out.println();
	System.out.println("based on code by: Oliver Labs and Richard Morris");
	System.out.println("idea: Oliver Labs (2001)");
	System.out.println();
	System.out.println("There are several ways of invoking surfex. The most common ways are:");
	System.out.println("surfex -e equation");
	System.out.println("surfex filename");
	System.out.println();
	System.out.println("E.g.: surfex niceSurface.sux");
	System.out.println("E.g.: surfex -e x^3+y^2-z^2");
	System.out.println();
	System.out.println("Attention: If your equation contains brackets or blanks, you might need to use quotation marks:");
	System.out.println("E.g.: surfex -e \"x^3+y^3+z^3+1-(x+y+z+1)^3\"");
	System.out.println("Notice: If a filename is given, the equation will not be used.");
	System.out.println();
	System.out.println("For additional help, see the doc directory or");
	System.out.println("go to our web-site:");
	System.out.println("http://www.surfex.AlgebraicSurface.net");
	System.out.println();
	System.out.println("The authors.");
	System.out.println();
   }
   
    public static void startProgressFrame(){
//   	pFrame.setAlwaysOnTop(true);
   	pFrame.setLocation(100,200);
	//Create and set up the content pane.
	
	JComponent newContentPane = progressFrame;;
	//newContentPane.setOpaque(true); //content panes must be opaque
	pFrame.setContentPane(newContentPane);
	
	//Display the window.
	pFrame.pack();
	pFrame.setVisible(true);
	
	progressFrame.refresh(5,"starting Surfex...");
    }
    
    
    public static void checkArgsForHelp(String[] args){
   	if (args.length == 1) {
	    if ((args[0].substring(0, 2)).equals("-h")
		|| (args[0].substring(0, 2)).equals("-?")) {
		showHelp();
		System.exit(0);
	    }
	}
	if (args.length == 3) {
	    if ((args[2].substring(0, 2)).equals("-h")
		|| (args[2].substring(0, 2)).equals("-?")
		|| (args[2].substring(0, 6)).equals("--help")) {
		showHelp();
		System.exit(0);
	    }
	}
	if (args.length == 5) {
	    if ((args[4].substring(0, 2)).equals("-h")
		|| (args[4].substring(0, 2)).equals("-?")
		|| (args[4].substring(0, 6)).equals("--help")) {
		showHelp();
		System.exit(0);
	    }
	}
    }
    
    public static void useCommands(String[] args){
	checkArgsForHelp(args);
	
	//////////////////////////////
	// use the command line parameters:
	boolean done = false;
	double outputWidth = 200;
	double outputHeight = 200;
	int outputDPI = 300;
	int outputColor = 0;
	int outputxrot=2;
	int outputyrot=1;
	int outputzrot=0;
	int outputLength=5;
	int outputOmitType=0;
	int outputFps=15;
	int outputFormat=0;
	
	boolean movie=false;
	boolean outputAntialiasing = false;
	String startFilename = "";
	String startEqnString = "";
	String unit="pixel";

	String rootFinder = "bezier_all_roots";
	String epsilon = "0.00000000000001";
	
	Vector runningParameters=new Vector();
	
	///////////////////////////////
	// create a new surfex object:
       //
       
       // schoin mal vorab checken ob ein Fenster geoefnet werden muss?
       boolean noWindow=false;
       for (int j = 0; j < args.length; j++) {
	   if (args[j].equals("-nw")
	       || args[j].equals("--nowindow")) {
	       noWindow = true;
	   }
       }
       if(!noWindow){
	   startProgressFrame();
	   progressFrame.refresh(5,"  -reading cmd-argumets");
	   progressFrame.refresh(50,"creating GUI");
       }
//  wird aber spaeter nochmal gescheckt
       
       boolean nw=false;
       for (int i = 0; i < args.length; i++) {
	   if(args[i].equals("-nw")
	      || args[i].equals("--nowindow")) {
	       nw = true;
	   }
       }
       surfex t = new surfex("surfex v. "+surfex.strVersion, null, false, "", "", "", nw);
       t.initFlag = true;
       
       // run through all the arguments
       // and treat them...
       // e.g., if the nowindow argument occurs
       // then we have to adapt the surfex call --> simply do not use .setVisible(true)
       // show some processing input on the command line
       
       
       for (int i = 0; i < args.length; i++) {
//	   System.out.println("arg "+i+":"+args[i]);
// 	   if(i<args.length-1) {
// 	       System.out.println("arg "+(i+1)+":"+args[i+1]);
// 	   }
	   if (args[i].equals("-h") || args[i].equals("-?")
	       || args[i].equals("--help")) {
	       showHelp();
	       System.exit(0);
	   }
	   if ((args[i].equals("-d") || args[i].equals("--directory"))
	       && args.length >= i + 1) {
	       t.currentDirectory = args[i + 1];
	       i++;
	   } else if ((args[i].equals("-t") || args[i].equals("--tempdir"))
		      && args.length >= i + 1) {
	       if(cygwin<=1) {
		   t.tmpDir = args[i + 1];
		   if (!(t.tmpDir.endsWith("/"))) {
		       t.tmpDir = t.tmpDir + "/";
		   }
	       }
	       i++;
	   } else if ((args[i].equals("-cyg") || args[i].equals("--cygdir"))
		      && args.length >= i + 1) {
	       t.cygdrivedir = args[i + 1];
	       if (!(t.cygdrivedir.endsWith("/"))) {
		   t.cygdrivedir = t.cygdrivedir + "/";
	       }
	       i++;
	   } else if ((args[i].equals("-i") || args[i].equals("--infile"))
		      && args.length >= i + 1) {
	       
	       startFilename = args[i + 1];
	       i++;
	   } else if ((args[i].equals("-o") || args[i].equals("--outfile"))
		      && args.length >= i + 1) {
	       t.outputFilename = args[i + 1];
	       i++;
	   } else if ((args[i].equals("-s") || args[i].equals("--size"))
		      && args.length >= i + 2) {
	       outputHeight = Double.parseDouble(args[i + 1]);
	       outputWidth = Double.parseDouble(args[i + 2]);
	       i += 2;
	   } else if ((args[i].equals("-r") || args[i].equals("--root_finder"))
		      && args.length >= i + 1) {
	       rootFinder = args[i + 1];
	       i += 1;
	   } else if ((args[i].equals("-eps") || args[i].equals("--epsilon"))
		      && args.length >= i + 1) {
	       epsilon = args[i + 1];
	       i += 1;
	   } else if (args[i].equals("--cm")) {
	       unit="cm";
	       //i ++;         
	   } else if (args[i].equals("--inch")) {
	       unit="inch";
	       //i ++;         
	   } else if (args[i].equals("--dpi") && args.length >= i + 1) {
	       outputDPI = Integer.parseInt(args[i + 1]);
	       i++;         
	   } else if (args[i].equals("-a")
		      || args[i].equals("--antialiasing")) {
	       outputAntialiasing = true;         
	   } else if (args[i].equals("--dither")) {
	       outputColor = 1;
	   } else if (args[i].equals("--3drg")) {
	       outputColor = 2;
	   } else if (args[i].equals("--3drb")) {
	       outputColor = 2;
	   } else if (args[i].equals("--surfcode")) {
	       outputFormat = 3;
	   } else if (args[i].equals("--povcode")) {
	       outputFormat=2;
	   } else if (args[i].equals("--fps")&& args.length >= i + 1) {
	       movie=true;
	       outputFps = Integer.parseInt(args[i + 1]);
	       i++;
	   }  else if (args[i].equals("--length")&& args.length >= i + 1) {
	       movie=true;
	       outputLength = Integer.parseInt(args[i + 1]);
	       i++;
	   }  else if (args[i].equals("--omit")&& args.length >= i + 1) {
	       movie=true;
	       outputOmitType = Integer.parseInt(args[i + 1]);
	       i++;
	   }  else if (args[i].equals("--runningParameter")&& args.length >= i + 1) {
	       //parameterToRun=true;
	       movie=true;
	       runningParameters.add(args[i + 1]);
	       i++;
	   } else if (args[i].equals("--showMoviePreview")) {
	       //parameterToRun=true;
	       t.showMoviePreview=true;
	   }
	   else if (args[i].equals("--rot")&& args.length >= i + 3) {
	       movie=true;
	       outputxrot = Integer.parseInt(args[i + 1]);
	       outputyrot = Integer.parseInt(args[i + 2]);
	       outputzrot = Integer.parseInt(args[i + 3]);
	       i+=3;
	   }
	   else if (args[i].equals("-q") || args[i].equals("--quit")) {
	       // quit after having produced a file
	       t.quitAfterwards = true;
	   } else if (args[i].equals("--movie")) {
	       movie=true;
	       t.quitAfterwards = true;
	   } else if ((args[i].equals("-e") || args[i].equals("--equation"))
		      && args.length >= i + 1) {
	       startEqnString = args[i + 1];
	       i++;
	   }else if(args[i].equals("-nw")
		    || args[i].equals("--nowindow")) {
	       t.noWindow = true;
	       t.quitAfterwards = true;
	   }else {
	       // if none of the others matches,  
	       // we expect the argument to be an equation:
	       startFilename = args[i];
	   }
       }
       
       // handle the arguments in a more intelligent way than the following:
       
       
       // resize if unit !=pixels:
       if(unit.equals("cm")){
	   outputWidth*=outputDPI/t.cm_inch_ratio;
	   outputHeight*=outputDPI/t.cm_inch_ratio;
       }else if(unit.equals("inch")){
	   outputWidth*=outputDPI;
	   outputHeight*=outputDPI;
       }

       if (!startFilename.equals("")) {
//	   t.closeProject(1);
	   File tempfile = new File(startFilename);
	   if (tempfile.exists()) {
	       t.load(startFilename);
	   } else {
	       t.load(t.currentDirectory + File.separator + startFilename);
	   }
	   t.raytrace();
	   done = true;
       } else {
	   if (!startEqnString.equals("")) {
	       ((Equation) ((t.getCurrentProject()).eqAdm.eqnList
			    .lastElement())).text.setText(startEqnString);
	       t.raytrace();
	       done = true;
	   }
       }

       (t.getCurrentProject()).setRootFinder(rootFinder);

       /*    if (!done) {
       // do some default thing...
       
       // assume that the argument is an equation:
       //    System.out.println("eqn:"+args[0]);
       ((Equation) ((t.getCurrentProject()).eqAdm.eqnList
       .lastElement())).text.setText(args[0]);
       t.raytrace();
       }*/
       
       if (!t.noWindow) {
	   t.showall();
	   t.getCurrentProject().startRaytraceAlways();
       }
       
       //System.out.println("outpname:"+t.outputFilename);
       if (t.quitAfterwards) {
	   if (!(t.outputFilename.equals(""))) {
	       // produce the output file and then exit
	       //        System.out.println("savePic...");
	       
	       Thread saveThread;
	       if(movie){
		   saveThread=new SaveMovie(t.currentDirectory + File.separator + t.outputFilename,
					    t,
					    outputFps,
					    outputDPI,(outputColor==1),
					    outputAntialiasing, outputLength,
					    outputOmitType,
					    (int)outputHeight, (int)outputWidth,
					    (outputColor==2 || outputColor==3 ), 
					    1, 
					    t.getCurrentProject(),
					    t.getCurrentProject().jv4sx.getCamPos(), 
					    t.getCurrentProject().jv4sx.getViewDir(),
					    t.getCurrentProject().jv4sx.getUpVector(),
					    t.getCurrentProject().jv4sx.getRightVector(),  
					    outputxrot, outputyrot, outputzrot,
					    paramsToString(runningParameters));      		  
		   
//	     System.out.println("save movie...");
	       } else {         
		   saveThread = new SavePic(t.currentDirectory + File.separator + t.outputFilename, 
					    (outputColor==1),
					    outputAntialiasing, 
					    (int)outputWidth, (int)outputHeight, outputDPI, 
					    t, 
					    t.getCurrentProject(),
					    t.getCurrentProject().jv4sx.getCamPos(),
					    t.getCurrentProject().jv4sx.getViewDir(),
					    t.getCurrentProject().jv4sx.getUpVector(),
					    t.getCurrentProject().jv4sx.getRightVector(),
					    t.getCurrentProject().parAdmin.getAllParams(),
					    t.getCurrentProject().parAdmin.getAllParamValues(),
					    jv4sx,
					    t.getCurrentProject().getAllLamps());
		   if(outputColor==2){
		       ((SavePic)saveThread).set3drg(true);
		   }if(outputColor==3){
		       ((SavePic)saveThread).set3drb(true);
		   }
		   //	     System.out.println("save pic...");
	       }
	       try {
		   saveThread.start();
		   saveThread.join();
	       } catch (InterruptedException e) {
		   
	       }
	       while (!t.getCurrentProject().saveDone) {
//	     wait(50);
	       }
	   }
	   
	   System.exit(0);
       }
       t.initFlag = false;
   }
    
    public static void noCommands(){
	// do some default thing...
	// ... nothing for now
	surfex t = new surfex("surfex v. "+surfex.strVersion, null, false, "", "", "", false);
    }
    
    public static void main(String[] args) {
	// only accept one single argument for the moment: -e<equation> or -i<filename>
	//  System.out.println("args:"+args.length);
	
	// Anzeigen welcher Teil der Gui gerade gestartet wird:
	//	if(args.)
		
	if (args.length == 0) {
	    startProgressFrame();
	    progressFrame.refresh(5,"reading cmd-arguments");
	    progressFrame.refresh(50,"creating GUI");
	    
	    noCommands();
	} else {
	    useCommands(args);
	}
    }
    
    public static String[] paramsToString(Vector v){
   	String[] s=new String[v.size()];
   	ListIterator li=v.listIterator();
   	int i=0;
   	while(li.hasNext()){
	    s[i++]=(String)li.next();
   	}
   	
   	return s;
    }
    
    
} // end class surfex

