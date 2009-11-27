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


import java.awt.Color;

//////////////////////////////////////////////////////////////
//
// class AppearanceScheme
//
//////////////////////////////////////////////////////////////
public class AppearanceScheme {
	//     Color []baseSurfaceColors = {new Color(0, 180, 240), new Color(240, 180, 0), new Color(240, 0, 180), 
	//         new Color(0, 240, 180), new Color(180, 240, 0), new Color(180, 0, 240)};
	Color[] baseSurfaceColors = { new Color(240, 160, 0),
			new Color(160, 240, 0), new Color(0, 160, 240),
			new Color(240, 0, 160), new Color(0, 240, 160),
			new Color(160, 0, 240) };

	double surfaceIntensityFactor = 0.8;

	Color[] baseCurveColors = { Color.black, Color.white };

	double curveIntensityFactor = 0.7;

	AppearanceScheme() {
	}

	public Color brighter(Color col, double factor, int no) {
		try {
			Color tmpColor = new Color(
					(int) (255 * (1 - (1 - col.getRed() / 255.0)
							* Math.pow(factor, no - 1))),
					(int) (255 * (1 - (1 - col.getGreen() / 255.0)
							* Math.pow(factor, no - 1))),
					(int) (255 * (1 - (1 - col.getBlue() / 255.0)
							* Math.pow(factor, no - 1))));
			//      System.out.println("col:"+col+",no"+no+", ---- newcol:"+tmpColor);
			return (tmpColor);
		} catch (Exception ex) {
			return (col);
		}
	}

	public Color darker(Color col, double factor, int no) {
		try {
			Color tmpColor = new Color((int) (col.getRed() * Math.pow(factor,
					no - 1)),
					(int) (col.getGreen() * Math.pow(factor, no - 1)),
					(int) (col.getBlue() * Math.pow(factor, no - 1)));
			//      System.out.println("col:"+col+",no"+no+", ---- newcol:"+tmpColor);
			return (tmpColor);
		} catch (Exception ex) {
			return (col);
		}
	}

	public Color getEquationColorInside(int no) {
		return (brighter(baseSurfaceColors[(no - 1)
				% (baseSurfaceColors.length)], surfaceIntensityFactor,
				((int) (no / (baseSurfaceColors.length))) * 3 + 2));
	}

	public Color getEquationColorOutside(int no) {
		return (brighter(baseSurfaceColors[(no - 1)
				% (baseSurfaceColors.length)], surfaceIntensityFactor,
				((int) (no / (baseSurfaceColors.length))) * 3 + 1));
	}

	public Color getCurveColor(int no) {
		if (no % 2 == 1) {
			return (brighter(baseCurveColors[(no - 1) % 2],
					curveIntensityFactor, ((int) ((no) / 2)) + 1));
		} else {
			return (darker(baseCurveColors[(no - 1) % 2], curveIntensityFactor,
					((int) ((no - 1) / 2)) + 1));
		}
	}
} // end class AppearanceScheme

