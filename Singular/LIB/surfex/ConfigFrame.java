////////////////////////////////////////////////////////////////////////
//
// This file ConfigFrame.java is part of SURFEX.
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

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Container;
import java.awt.GridLayout;
import java.awt.Panel;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;

import javax.swing.AbstractButton;
import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JCheckBox;
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
// class ConfigFrame
//
//////////////////////////////////////////////////////////////

public class ConfigFrame extends JFrame {

	JPanel panel, panel2, panel3, panel4, panel5, panel6, panel7;

	surfex surfex_;

	JCheckBox rayImme = new JCheckBox("raytrace preview immediately", false);

	//UpdateRayframeImmediatlyThread updateRayframeImmediatlyThread;

	JTextField interpolConst;

	JTextField povPath = new JTextField();

	JTextField surfPath = new JTextField();

	JTextField impsurfPath = new JTextField();

	JTextField iviewPath = new JTextField();

	JTextField serverPath = new JTextField();

	JRadioButton surf, pov, localPro, webPro;

	boolean last[] = { true, false, false, true, false };

	ChangeListener CL = new ChangeListener() {
		public void stateChanged(ChangeEvent ev) {
			if (((AbstractButton) ev.getSource()).getText().hashCode() == rayImme
					.getText().hashCode()) {
				if (rayImme.isSelected() && !last[4]
						&& surfex_.pane.getSelectedIndex() >= 0) {
					last[4] = true;
					double ang[] = surfex_.getCurrentProject().eqAdm
							.getAngles();
					double scale[] = surfex_.getCurrentProject().eqAdm
							.getScales();
					surfex_.newUpdateRayframeImmediatlyThread();
					surfex_.updateRayframeImmediatlyThread.start();
				}
				if (!rayImme.isSelected() && last[4]) {
					last[4] = false;
					surfex_.updateRayframeImmediatlyThread.stop();
				}
			}

			if (((AbstractButton) ev.getSource()).getText().hashCode() == localPro
					.getText().hashCode()) {

				// checke ob Programme installiert sind
				if (localPro.isSelected() && !last[0]) {
					last[0] = true;
					if (pov.isSelected() && !povAvailable()) {
						ask4pov2();

					} else {
						if (surf.isSelected() && !surfAvailable()) {
							ask4surf2();
						}
					}
					last[0] = localPro.isSelected();
					last[1] = webPro.isSelected();
				} else {
					// webPro wurde selected()
					last[0] = localPro.isSelected();
					last[1] = webPro.isSelected();
				}
			}
			if (((AbstractButton) ev.getSource()).getText().hashCode() == surf
					.getText().hashCode()) {
				if (surf.isSelected() && !last[2]) {
					last[2] = true;
					// voruebergehend, damit  er nicht 10x den dialog aufruft, weil man die maus bewegt
					// wurde auf surf geclickt
					if (localPro.isSelected() && !surfAvailable()) {
						// soll also local verfuegbar sein, isses abernet
						ask4surf2();
					}
					last[2] = surf.isSelected();
					last[3] = pov.isSelected();
				} else {
					if (pov.isSelected() && !last[3]) {
						last[3] = true;
						// voruebergehend, damit  er nicht 10x den dialog aufruft, weil man die maus bewegt

						// wurde auf surf geclickt
						if (localPro.isSelected() && !povAvailable()) {
							// soll also local verfuegbar sein, isses abernet
							ask4pov2();
						}
						last[2] = surf.isSelected();
						last[3] = pov.isSelected();
					}
				}

			}
		}
	};

	ActionListener AL = new ActionListener() {
		public void actionPerformed(ActionEvent actionEvent) {
			action(actionEvent.getActionCommand());
		}
	};

	public void action(String command) {
		if (command == "OK") {
			setVisible(false);
		}
		if (command == "povPathButton") {
			filechooser("POV", ".exe");
		}
		if (command == "surfPathButton") {
			filechooser("surf", ".exe");
		}
		if (command == "impsurfPathButton") {
			filechooser("impsurf", ".exe");
		}
		if (command == "iviewPathButton") {
			filechooser("iview", ".exe");
		}
		if (command == "serverPath") {
			//filechooser("iview", ".exe");
		}
		//serverPath
		/*
		 if(command == "Default"){

		 } */
	}

	ConfigFrame(String pPath, String sPath, String imPath, String ivPath,
			int iConst, String svPath, UpdateRayframeImmediatlyThread uT,
			surfex su) {
		super("Config surfex");
		surfex_ = su;
		//updateRayframeImmediatlyThread = uT;
		setSize(300, 350);
		interpolConst = new JTextField((new Integer(iConst)).toString(), 1);
		setLocation(100, 100);
		Container cp = getContentPane();
		cp.setLayout(new BorderLayout());
		panel = new JPanel(new GridLayout(5, 1));

		//  panel.add(rayImme);
		rayImme.addChangeListener(CL);

		panel2 = new JPanel(new GridLayout(1, 2));
		panel2.add(new JLabel("     interpolation const."));
		panel2.add(interpolConst);
		panel.add(panel2);

		panel3 = new JPanel(new GridLayout(1, 3));
		panel3.add(new JLabel("     raytracer"));
		surf = new JRadioButton("surf", true);
		surf.addChangeListener(CL);
		pov = new JRadioButton("POVray");
		pov.addChangeListener(CL);
		ButtonGroup group = new ButtonGroup();
		group.add(surf);
		panel3.add(surf);
		if (!(surfex_.inAnApplet)) {
			group.add(pov);
			panel3.add(pov);
		}
		panel.add(panel3);

		ButtonGroup group2 = new ButtonGroup();

		if (surfex_.inAnApplet) {
			webPro = new JRadioButton("use web Programs", true);
			localPro = new JRadioButton("use local Programs");
			localPro.setEnabled(false);
		} else {
			webPro = new JRadioButton("use web Programs");
			localPro = new JRadioButton("use local Programs", true);
		}
		localPro.addChangeListener(CL);
		group2.add(localPro);
		group2.add(webPro);
		panel.add(webPro);
		panel.add(localPro);

		panel4 = new JPanel(new BorderLayout());
		panel5 = new JPanel(new GridLayout(6, 1));
		panel6 = new JPanel(new GridLayout(6, 1));
		panel7 = new JPanel(new GridLayout(6, 1));

		panel5.add(new JLabel("POV:"));
		povPath.setText(pPath);
		boolean temp = povAvailable();
		panel6.add(povPath);
		JButton povPathButton = new JButton("choose");
		povPathButton.setActionCommand("povPathButton");
		povPathButton.addActionListener(AL);
		panel7.add(povPathButton);

		panel5.add(new JLabel("SURF:"));
		surfPath.setText(sPath);
		temp = surfAvailable();
		panel6.add(surfPath);
		JButton surfPathButton = new JButton("choose");
		surfPathButton.setActionCommand("surfPathButton");
		surfPathButton.addActionListener(AL);
		panel7.add(surfPathButton);

		panel5.add(new JLabel("Impsurf:"));
		impsurfPath.setText(imPath);
		temp = impsurfAvailable();
		panel6.add(impsurfPath);
		JButton impsurfPathButton = new JButton("choose");
		impsurfPathButton.setActionCommand("impsurfPathButton");
		impsurfPathButton.addActionListener(AL);
		panel7.add(impsurfPathButton);

		panel5.add(new JLabel("Iview:"));
		iviewPath.setText(ivPath);
		temp = iviewAvailable();
		panel6.add(iviewPath);
		JButton iviewPathButton = new JButton("choose");
		iviewPathButton.setActionCommand("iviewPathButton");
		iviewPathButton.addActionListener(AL);
		panel7.add(iviewPathButton);

		panel5.add(new JLabel("Server:"));
		serverPath.setText(svPath);
		temp = serverAvailable();
		panel6.add(serverPath);
		JButton serverPathButton = new JButton("choose");
		serverPathButton.setActionCommand("serverPathButton");
		serverPathButton.addActionListener(AL);
		panel7.add(serverPathButton);

		panel5.add(new JLabel());
		JButton OKButton = new JButton("OK");
		OKButton.addActionListener(AL);
		panel6.add(OKButton);
		/*JButton cancelButton=new JButton("Cancel");
		 OKButton.addActionListener(AL);
		 */
		panel7.add(new JLabel());//cancelButton);

		if (!(surfex_.inAnApplet)) {
			panel4.add(panel5, BorderLayout.WEST);
			panel4.add(panel6, BorderLayout.CENTER);
			panel4.add(panel7, BorderLayout.EAST);
		}

		cp.add(panel4, BorderLayout.SOUTH);
		cp.add(panel, BorderLayout.CENTER);

		setVisible(false);

	}

	public boolean povAvailable() {
		try {
			Runtime r = Runtime.getRuntime();
			Process p;
			p = r.exec(povPath.getText()
					+ " /NR /RENDER tmp_aviable_test.povtest /EXIT");
			povPath.setBackground(Color.white);
			return true;
		} catch (Exception e1) {
			//      System.out.println(e1.toString());
			povPath.setBackground(Color.red);
			return (false);
		}
		//return false;
	}

	public boolean surfAvailable() {
		try {
			Runtime r = Runtime.getRuntime();
			Process p;
			p = r.exec(surfPath.getText() + " testparameter");
			surfPath.setBackground(Color.white);
			return true;
		} catch (Exception e1) {
			//      System.out.println(e1.toString());
			surfPath.setBackground(Color.red);
			return (false);
		}
	}

	public boolean serverAvailable() {
		/*try {
		 Runtime r = Runtime.getRuntime();
		 Process p;
		 p = r.exec(surfPath.getText() + " testparameter");
		 surfPath.setBackground(Color.white);
		 return true;
		 } catch (Exception e1) {
		 //    System.out.println(e1.toString());
		 surfPath.setBackground(Color.red);
		 return (false);
		 } */
		return true;
	}

	public boolean impsurfAvailable() {
		try {
			Runtime r = Runtime.getRuntime();
			Process p;
			p = r.exec(impsurfPath.getText() + " testparameter");
			impsurfPath.setBackground(Color.white);
			return true;
		} catch (Exception e1) {
			//      System.out.println(e1.toString());
			impsurfPath.setBackground(Color.red);
			return (false);
		}
	}

	public boolean iviewAvailable() {
		try {
			Runtime r = Runtime.getRuntime();
			Process p;
			p = r.exec(iviewPath.getText() + " i_view32_test.jpg /convert="
					+ surfex_.tmpDir + "i_view32_test.jpg");
			iviewPath.setBackground(Color.white);
			return true;
		} catch (Exception e1) {
			//      System.out.println(e1.toString());
			iviewPath.setBackground(Color.red);
			return (false);
		}
	}

	public void askForSurf() {

		int optReturn = JOptionPane.showConfirmDialog(new Panel(),
				"Can't find POV - do you want me to try using surf?",
				"Can't find POV", JOptionPane.YES_NO_CANCEL_OPTION,
				JOptionPane.QUESTION_MESSAGE);
		switch (optReturn) {
		case 0:
			// ja, dann nutze surf
			surf.setSelected(true);
			if (!surfAvailable()) {
				// der Pfad war wohl nich korrekt
				optReturn = JOptionPane
						.showConfirmDialog(
								new Panel(),
								"Can't find SURF - do you konow where SURF is installed?",
								"Can't find SURF",

								JOptionPane.YES_NO_CANCEL_OPTION,
								JOptionPane.QUESTION_MESSAGE);
				switch (optReturn) {
				case 0:
					// ja, dann suche surf
					// creating new Frame
					final JFrame frame2 = new JFrame("load surf");

					// adding container
					Container contentPane = frame2.getContentPane();

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
										+ File.separator
										+ selectedFile.getName());

								// ausgabe fehler bein z.B.: d:\ -> d:\\ bzw \ -> \\
								//          System.out.println("filelocation : "+ filelocation + "\n");

								povPath.setText(filelocation);

							} else {
								//if(command.equals(JFileChooser.CANCEL_SELECTION)){
								frame2.setVisible(false);
								//}
							}
							if (!surfAvailable()) {
								//falsch
								System.out
										.println(" dann eghts halt nur mitm Netz");
								webPro.setSelected(true);
							}
						}
					};

					FileFilter acfFilter = new ExtensionFileFilter(
							"all common files", new String[] { "exe" });
					fileChooser.addChoosableFileFilter(acfFilter);

					FileFilter spcFilter = new ExtensionFileFilter("*.exe",
							new String[] { "exe" });
					fileChooser.addChoosableFileFilter(spcFilter);

					// adding actionListener
					fileChooser.addActionListener(actionListener);
					frame2.pack();
					frame2.setVisible(true);

					break;
				case 1:
					//nein
					System.out.println(" dann eghts halt nur mitm Netz");
					webPro.setSelected(true);
					break;
				case 2: // abbrechen, also nic machen ...
					break;
				}
				;
			}
			break;
		case 1:
			//nein
			System.out.println(" dann eghts halt nur mitm Netz");
			webPro.setSelected(true);
			break;
		case 2: // abbrechen, also nic machen ...
			break;
		}
		;

	}

	public void askForPOV() {
		// der Pfad war wohl nich korrekt
		int optReturn = JOptionPane.showConfirmDialog(new Panel(),
				"Can't find SURF - do you want me to try using POV?",
				"Can't find SURF", JOptionPane.YES_NO_CANCEL_OPTION,
				JOptionPane.QUESTION_MESSAGE);
		switch (optReturn) {
		case 0:
			// ja, dann nutze POV
			pov.setSelected(true);
			if (!povAvailable()) {
				// der Pfad war wohl nich korrekt
				optReturn = JOptionPane
						.showConfirmDialog(
								new Panel(),
								"Can't find POV - do you konow where POV is installed2?",
								"Can't find POV",

								JOptionPane.YES_NO_CANCEL_OPTION,
								JOptionPane.QUESTION_MESSAGE);
				switch (optReturn) {
				case 0:
					// ja, dann nutze POV
					// creating new Frame
					final JFrame frame2 = new JFrame("load POV");

					// adding container
					Container contentPane = frame2.getContentPane();

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
										+ File.separator
										+ selectedFile.getName());

								// ausgabe fehler bein z.B.: d:\ -> d:\\ bzw \ -> \\
								//          System.out.println("filelocation : "+ filelocation + "\n");

								povPath.setText(filelocation);

							} else {
								//if(command.equals(JFileChooser.CANCEL_SELECTION)){
								frame2.setVisible(false);
								//}
							}
							if (!povAvailable()) {
								//falsch
								//              System.out.println(" dann eghts halt nur mitm Netz");
								webPro.setSelected(true);
							}
						}
					};

					FileFilter acfFilter = new ExtensionFileFilter(
							"all common files", new String[] { "exe" });
					fileChooser.addChoosableFileFilter(acfFilter);

					FileFilter spcFilter = new ExtensionFileFilter("*.exe",
							new String[] { "exe" });
					fileChooser.addChoosableFileFilter(spcFilter);

					// adding actionListener
					fileChooser.addActionListener(actionListener);
					frame2.pack();
					frame2.setVisible(true);

					break;
				case 1:
					//nein
					//          System.out.println(" dann eghts halt nur mitm Netz");
					webPro.setSelected(true);
					break;
				case 2: // abbrechen, also nic machen ...
					break;
				}
				;
			}

			break;
		case 1:
			//nein
			//    System.out.println(" dann eghts halt nur mitm Netz");
			webPro.setSelected(true);
			break;
		case 2: // abbrechen, also nic machen ...
			break;
		}
		;
	}

	public void ask4surf2() {
		int optReturn = JOptionPane.showConfirmDialog(new Panel(),
				"Can't find surf - do yout know where surf is installed?",
				"Can't find surf", JOptionPane.YES_NO_CANCEL_OPTION,
				JOptionPane.QUESTION_MESSAGE);
		switch (optReturn) {
		case 0:
			// ja, dann gib den Pfad an
			// creating new Frame
			final JFrame frame2 = new JFrame("load surf");

			// adding container
			Container contentPane = frame2.getContentPane();

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
						File selectedFile = theFileChooser.getSelectedFile();

						// making savepopup invisible/removing it
						frame2.dispose();

						// creating complete filepath
						String filelocation = new String(selectedFile
								.getParent()
								+ File.separator + selectedFile.getName());

						// ausgabe fehler bein z.B.: d:\ -> d:\\ bzw \ -> \\
						//          System.out.println("filelocation : "+ filelocation + "\n");

						surfPath.setText(filelocation);

					} else {
						//if(command.equals(JFileChooser.CANCEL_SELECTION)){
						frame2.setVisible(false);
						//}
					}
					if (surf.isSelected() && !surfAvailable()) {
						// der Pfad war wohl nich korrekt
						askForPOV();
					}
				}
			};

			FileFilter acfFilter = new ExtensionFileFilter("all common files",
					new String[] { "exe" });
			fileChooser.addChoosableFileFilter(acfFilter);

			FileFilter spcFilter = new ExtensionFileFilter("*.exe",
					new String[] { "exe" });
			fileChooser.addChoosableFileFilter(spcFilter);

			// adding actionListener
			fileChooser.addActionListener(actionListener);
			frame2.pack();
			frame2.setVisible(true);

			break;
		case 1:
			// surf.setSelected(true);
			askForPOV();
			break;
		case 2: // abbrechen, also nic machen ...
			break;
		}
		;

	}

	public void ask4pov2() {
		int optReturn = JOptionPane.showConfirmDialog(new Panel(),
				"Can't find POV - do yout know where POV is installed?",
				"Can't find POV", JOptionPane.YES_NO_CANCEL_OPTION,
				JOptionPane.QUESTION_MESSAGE);
		switch (optReturn) {
		case 0:
			// ja, dann gib den Pfad an
			// creating new Frame
			final JFrame frame2 = new JFrame("load POV");

			// adding container
			Container contentPane = frame2.getContentPane();

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
						File selectedFile = theFileChooser.getSelectedFile();

						// making savepopup invisible/removing it
						frame2.dispose();

						// creating complete filepath
						String filelocation = new String(selectedFile
								.getParent()
								+ File.separator + selectedFile.getName());

						// ausgabe fehler bein z.B.: d:\ -> d:\\ bzw \ -> \\
						//          System.out.println("filelocation : "+ filelocation + "\n");

						povPath.setText(filelocation);

					} else {
						//if(command.equals(JFileChooser.CANCEL_SELECTION)){
						frame2.setVisible(false);
						//}
					}
					if (pov.isSelected() && !povAvailable()) {
						// der Pfad war wohl nich korrekt
						askForSurf();
					}
				}
			};

			FileFilter acfFilter = new ExtensionFileFilter("all common files",
					new String[] { "exe" });
			fileChooser.addChoosableFileFilter(acfFilter);

			FileFilter spcFilter = new ExtensionFileFilter("*.exe",
					new String[] { "exe" });
			fileChooser.addChoosableFileFilter(spcFilter);

			// adding actionListener
			fileChooser.addActionListener(actionListener);
			frame2.pack();
			frame2.setVisible(true);

			break;
		case 1:
			// surf.setSelected(true);
			askForSurf();
			break;
		case 2: // abbrechen, also nic machen ...
			break;
		}
	}

	public void filechooser(final String applicationName, String endung) {

		// creating new Frame
		final JFrame frame2 = new JFrame("load " + applicationName);

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
					File selectedFile = theFileChooser.getSelectedFile();

					// making savepopup invisible/removing it
					frame2.dispose();

					// creating complete filepath
					String filelocation = new String(selectedFile.getParent()
							+ File.separator + selectedFile.getName());

					// ausgabe fehler bein z.B.: d:\ -> d:\\ bzw \ -> \\
					//          System.out.println("filelocation : "+ filelocation + "\n");

					if (applicationName == "POV") {
						povPath.setText(filelocation);
						boolean temp = povAvailable();
					}
					if (applicationName == "surf") {
						surfPath.setText(filelocation);
						boolean temp = surfAvailable();
					}
					if (applicationName == "impsurf") {
						impsurfPath.setText(filelocation);
						boolean temp = impsurfAvailable();
					}
					if (applicationName == "iview") {
						iviewPath.setText(filelocation);
						boolean temp = iviewAvailable();
					}
				} else {
					//if(command.equals(JFileChooser.CANCEL_SELECTION)){
					frame2.setVisible(false);
					//}
				}
			}
		};

		FileFilter acfFilter = new ExtensionFileFilter("all common files",
				new String[] { endung });
		fileChooser.addChoosableFileFilter(acfFilter);

		FileFilter spcFilter = new ExtensionFileFilter("*." + endung,
				new String[] { endung });
		fileChooser.addChoosableFileFilter(spcFilter);

		// adding actionListener
		fileChooser.addActionListener(actionListener);
		frame2.pack();
		frame2.setVisible(true);

		//oeffen den Kram unter filename;

	} // end: command == "load"
} // end class ConfigFrame

