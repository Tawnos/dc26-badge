SamacSys ECAD Model
265866/28562/2.18/2/5/Ferrite Bead

DESIGNSPARK_INTERMEDIATE_ASCII

(asciiHeader
	(fileUnits MM)
)
(library Library_1
	(padStyleDef "r100_85"
		(holeDiam 0)
		(padShape (layerNumRef 1) (padShapeType Rect)  (shapeWidth 0.85) (shapeHeight 1))
		(padShape (layerNumRef 16) (padShapeType Ellipse)  (shapeWidth 0) (shapeHeight 0))
	)
	(textStyleDef "Default"
		(font
			(fontType Stroke)
			(fontFace "Helvetica")
			(fontHeight 50 mils)
			(strokeWidth 5 mils)
		)
	)
	(patternDef "BEADC1608X75N" (originalName "BEADC1608X75N")
		(multiLayer
			(pad (padNum 1) (padStyleRef r100_85) (pt -0.8, 0) (rotation 0))
			(pad (padNum 2) (padStyleRef r100_85) (pt 0.8, 0) (rotation 0))
		)
		(layerContents (layerNumRef 18)
			(attr "RefDes" "RefDes" (pt 0, 0) (textStyleRef "Default") (isVisible True))
		)
		(layerContents (layerNumRef 30)
			(line (pt -1.475 0.75) (pt 1.475 0.75) (width 0.05))
		)
		(layerContents (layerNumRef 30)
			(line (pt 1.475 0.75) (pt 1.475 -0.75) (width 0.05))
		)
		(layerContents (layerNumRef 30)
			(line (pt 1.475 -0.75) (pt -1.475 -0.75) (width 0.05))
		)
		(layerContents (layerNumRef 30)
			(line (pt -1.475 -0.75) (pt -1.475 0.75) (width 0.05))
		)
		(layerContents (layerNumRef 28)
			(line (pt -0.8 0.4) (pt 0.8 0.4) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt 0.8 0.4) (pt 0.8 -0.4) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt 0.8 -0.4) (pt -0.8 -0.4) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt -0.8 -0.4) (pt -0.8 0.4) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt 0 0.3) (pt 0 -0.3) (width 0.2))
		)
	)
	(symbolDef "BLM18TG221TN1D" (originalName "BLM18TG221TN1D")

		(pin (pinNum 1) (pt 0 mils 0 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinDes (text (pt 175 mils 0 mils) (rotation 0) (justify "Right") (textStyleRef "Default"))) (pinName (text (pt 225 mils -25 mils) (rotation 0) (justify "Left") (textStyleRef "Default"))
		))
		(pin (pinNum 2) (pt 700 mils 0 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinDes (text (pt 525 mils 0 mils) (rotation 0) (justify "Left") (textStyleRef "Default"))) (pinName (text (pt 500 mils -25 mils) (rotation 0) (justify "Right") (textStyleRef "Default"))
		))

		(line (pt 200 mils 100 mils) (pt 500 mils 100 mils) (width 10 mils))
		(line (pt 500 mils 100 mils) (pt 500 mils -100 mils) (width 10 mils))
		(line (pt 500 mils -100 mils) (pt 200 mils -100 mils) (width 10 mils))
		(line (pt 200 mils -100 mils) (pt 200 mils 100 mils) (width 10 mils))

		(attr "RefDes" "RefDes" (pt 550 mils 350 mils) (isVisible True) (textStyleRef "Default"))

	)

	(compDef "BLM18TG221TN1D" (originalName "BLM18TG221TN1D") (compHeader (numPins 2) (numParts 1) (refDesPrefix FB)
		)
		(compPin "1" (pinName "1") (partNum 1) (symPinNum 1) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "2" (pinName "2") (partNum 1) (symPinNum 2) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(attachedSymbol (partNum 1) (altType Normal) (symbolName "BLM18TG221TN1D"))
		(attachedPattern (patternNum 1) (patternName "BEADC1608X75N")
			(numPads 2)
			(padPinMap
				(padNum 1) (compPinRef "1")
				(padNum 2) (compPinRef "2")
			)
		)
		(attr "Supplier_Name" "RS")
		(attr "RS Part Number" "6197526")
		(attr "Manufacturer_Name" "Murata Electronics")
		(attr "Manufacturer_Part_Number" "BLM18TG221TN1D")
		(attr "Allied_Number" "")
		(attr "Other Part Number" "")
		(attr "Description" "Ferrite bead SMD 0603 220R Murata Ferrite Bead (Chip Ferrite Bead), 1.6 x 0.8 x 0.6mm (0603), 1000 impedance at 100 MHz")
		(attr "Datasheet Link" "http://www.murata.com/~/media/webrenewal/support/library/catalog/products/emc/emifil/c31e.ashx?la=en-gb")
		(attr "Height" "0.75 mm")
		(attr "3D Package" "")
	)

)
