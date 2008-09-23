////////////////////////////////////////////////////////////////////////
//
// This file UpdateRayframeImmediatlyThread.java is part of SURFEX.
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

import java.util.*;

public class UpdateRayframeImmediatlyThread extends Thread implements Runnable {
	Project project;

	RayFrame rayFrame;

	surfex surfex_;

	Thread t;

	String old_surfCode = "";

	jv4surfex jv4sx;
	jv4surfex jv4sx_project;

	boolean killMe = false;
 
	SavePic saveThread;

	boolean showOnlySelectedLamp = false;

	int rand;

	Vector Lamps;

	//boolean only
	Lamp selectedLamp;

	UpdateRayframeImmediatlyThread(RayFrame ray, surfex su, Project pro,
			jv4surfex jv4sx) {
		rayFrame = ray;
		this.jv4sx = jv4sx;
		surfex_ = su;
		project = pro;
		rand = (int) ((Math.random()) * 2000000000);
		//System.out.println(rand);
		Lamps = project.getAllLamps();
//		System.out.println("pla1");
	}

	UpdateRayframeImmediatlyThread(RayFrame ray, surfex su, Project pro,
			jv4surfex jv4sx,jv4surfex jv4sx_project, Vector Lamps, boolean showOnlySelectedLamp,
			Lamp selectedLamp) {
//		System.out.println("pla2");
		rayFrame = ray;
		this.jv4sx = jv4sx;
		this.jv4sx_project = jv4sx_project;
		surfex_ = su;
		project = pro;
		rand = (int) ((Math.random()) * 2000000000);
//		System.out.println(rand);
		// denn es soll nureine Lampe an sein!
		this.Lamps = Lamps;
		//Lamps.add(lamp);
		this.selectedLamp = selectedLamp;
		this.showOnlySelectedLamp = showOnlySelectedLamp;
		
	}
	


	public void start() {
		if (t == null) {
			t = new Thread(this);
			t.start(); 
		}
	}

	/*
	 * public void stop() { killMe = true; t = null; //
	 * System.out.println("comittsuicide" ); }
	 */

	public void run() {
		Thread me = Thread.currentThread();
		String fn = surfex_.tmpDir + "uit" + rand + "test.png";
		saveThread = new SavePic(fn, false, project.antialiasing.isSelected(),
					 rayFrame.panel.getSize().height
					 / project.getInterpolConst(), 
					 rayFrame.panel.getSize().width
					 / project.getInterpolConst(), 
					 72, surfex_, project, jv4sx.getCamPos(), jv4sx.getViewDir(), jv4sx.getUpVector(),
					 jv4sx.getRightVector(),project.parAdmin.getAllParams(),project.parAdmin.getAllParamValues(), 
					 jv4sx, Lamps);
		// falls nur eine Lampe angezeigt werden soll:;
		if (surfex_.configFrame.surf.isSelected()) {
			fn = surfex_.tmpDir + project.tmp_surfex_file + "uit"
					+ rand + ".jpg";
		} else {
			fn = surfex_.tmpDir + "uit" + rand + "test.png";
		}
	

		
		// System.out.println("Test");
		while (me == t) {
			synchronized (this) {
				
				updateLamps();

				if (project.equals(surfex_.getCurrentProject())
						&& rayFrame.isVisible()) {

				//	System.out.println("visible:"+rand+"ray:"+rayFrame.rand);
					// nur neu berechnen, wenn dies das aktuelle project ist
					// und auch sichtbar ist
					
					// damit das Programm noch reagiert);
					/*
					 * try { sleep(100); } catch (InterruptedException e) { }
					 */
			
					// das muss hier stehen, weil sonst aus irgendwelchen Gruenden
					// das normale previewFenster nicht mitupdated
					// wenn man Lampen nur an und aus schaltet
					saveThread = new SavePic(
							fn,
							false,
							project.antialiasing.isSelected(),
							rayFrame.panel.getSize().height
									/ project.getInterpolConst(),
							rayFrame.panel.getSize().width
									/ project.getInterpolConst(), 72, 
							surfex_, project, jv4sx.getCamPos(), jv4sx
									.getViewDir(), jv4sx.getUpVector(),
							jv4sx.getRightVector(),project.parAdmin.getAllParams(),project.parAdmin.getAllParamValues(), jv4sx, Lamps, jv4sx_project);
					
					//int i=System.in.readln();
					// System.out.println("1" );
					if (saveThread.getSurfCode().equals(old_surfCode)) {
						//if
						// (saveThread.getSurfCode().equals(old_surfCodePart1))
						// {
						// we do not have to recompute the same image!
						try {
							sleep(30);
						} catch (Exception e) {
							System.out.println("exception in UpdateRayFrameImmediatlythread jfdjgeig");
						}
					} else {
						//	System.out.println("do
						// recompute!"+this.showOnlySelectedLamp);
						//System.out.println(project.rand + " "
						//		+ project.equals(surfex_.getCurrentProject()));

						rayFrame.isWorking(true); // anzeigen, dass er arbeitet
						old_surfCode = saveThread.getSurfCode();

						try {

							saveThread.start();
							saveThread.join();
						} catch (InterruptedException e) {

						}
						
						// falls nur eine Lampe angezeigt werden soll:;
						//			      System.out.println("finally");
						Runtime r = Runtime.getRuntime();
						Process p;
						try {
							if (project.getInterpolConst() != 1) {
								if (surfex_.OS == surfex_.osWINDOWS) {
// 									p = r.exec(surfex_.configFrame.iviewPath
// 											.getText()
// 											+ " "
// 											+ surfex_.tmpDir
// 											+ "uit"
// 											+ rand
// 											+ "test.png /resample=(0,"
// 											+ rayFrame.getContentPane()
// 													.getSize().height
// 											+ ") /convert="
// 											+ surfex_.tmpDir
// 											+ "uit" + rand + "test.png");
// 									p.waitFor();
								} else {
									// ??? unter Linux auch interpolieren?
								}
							} else {
								//           p = r
								//               .exec("iview\\i_view32.exe test.bmp
								// /convert=test.png");

							}

						} catch (Exception er) {
							System.out.println(er);
						}
						if(surfex_.cygwin==1) {
						    rayFrame.changeBackground(surfex.toWindows(fn));
						} else {
						    rayFrame.changeBackground(fn);
						}
						rayFrame.isWorking(false);
					}

					// thread ist fertig

					//  SwingUtilities.updateComponentTreeUI(bt4);
				} else {
					
					// dieses Project ist nicht das aktuell betrachtete -> nichts machen
					try {
						sleep(50);
					} catch (Exception e) {
						System.out.println("exception in UpdateRayFrameImmediatlythread jfdjgeig");
					}
				}
			}
		}
	}

	public String vec2Str(double[] v) {
		return ("<" + v[0] + "," + v[1] + "," + v[2] + ">");
	}

	public synchronized void updateLamps() {
		if (showOnlySelectedLamp) {
			// update selected Lamp:
			if (project.lampAdmin != null && project.lampAdmin.lampIsSelected) {
				//project.lampAdmin
					//	.getSelectedLamp()
						//.updateKoords_InvokedByUpdateFrameImmediatlyThread_LampAdmin(
							//	jv4sx);//getCameraRotationYXZ());
			} // 	System.out.println(selectedLamp+" "+jv4sx);
			//   System.out.println("update selected Lamp");
			/*
			 * System.out.println("\n a"+jv4sx.getCameraRotationYXZ()[0]);
			 * System.out.println("b"+jv4sx.getCamPos()[0]);
			 * System.out.println("c"+jv4sx.getRightVector()[0]);
			 * System.out.println("d"+jv4sx.getUpVector()[0]);
			 * System.out.println("e"+jv4sx.getViewDir()[0]);
			 */// Lamps=new Vector();
			// Lamps.add(selectedLamp);
			//}
			
		//	saveThread.updateLamps(Lamps);
		} else {
			// 
			//System.out.println(rand);
			Lamps = project.getAllLamps();
			//System.out.println("s" + vec2Str(jv4sx.getCamPos()));

		}
	}
} // end class UpdateRayframeImmediatlyThread

