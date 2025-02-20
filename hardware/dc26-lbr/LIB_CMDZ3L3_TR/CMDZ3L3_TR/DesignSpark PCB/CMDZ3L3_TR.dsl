SamacSys ECAD Model
792590/28562/2.18/2/4/Zener Diode

DESIGNSPARK_INTERMEDIATE_ASCII

(asciiHeader
	(fileUnits MM)
)
(library Library_1
	(padStyleDef "r105_55"
		(holeDiam 0)
		(padShape (layerNumRef 1) (padShapeType Rect)  (shapeWidth 0.55) (shapeHeight 1.05))
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
	(patternDef "SOD2512X110N" (originalName "SOD2512X110N")
		(multiLayer
			(pad (padNum 1) (padStyleRef r105_55) (pt -1.15, 0) (rotation 90))
			(pad (padNum 2) (padStyleRef r105_55) (pt 1.15, 0) (rotation 90))
		)
		(layerContents (layerNumRef 18)
			(attr "RefDes" "RefDes" (pt 0, 0) (textStyleRef "Default") (isVisible True))
		)
		(layerContents (layerNumRef 30)
			(line (pt -1.925 1.15) (pt 1.925 1.15) (width 0.05))
		)
		(layerContents (layerNumRef 30)
			(line (pt 1.925 1.15) (pt 1.925 -1.15) (width 0.05))
		)
		(layerContents (layerNumRef 30)
			(line (pt 1.925 -1.15) (pt -1.925 -1.15) (width 0.05))
		)
		(layerContents (layerNumRef 30)
			(line (pt -1.925 -1.15) (pt -1.925 1.15) (width 0.05))
		)
		(layerContents (layerNumRef 28)
			(line (pt -0.85 0.625) (pt 0.85 0.625) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt 0.85 0.625) (pt 0.85 -0.625) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt 0.85 -0.625) (pt -0.85 -0.625) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt -0.85 -0.625) (pt -0.85 0.625) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt -0.85 0.1) (pt -0.325 0.625) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt -1.675 0.625) (pt 0.85 0.625) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt -0.85 -0.625) (pt 0.85 -0.625) (width 0.2))
		)
	)
	(symbolDef "CMDZ3L3_TR" (originalName "CMDZ3L3_TR")

		(pin (pinNum 1) (pt 0 mils 0 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName false)) (pinName (text (pt 0 mils -45 mils) (rotation 0]) (justify "UpperLeft") (textStyleRef "Default"))
		))
		(pin (pinNum 2) (pt 600 mils 0 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName false)) (pinName (text (pt 600 mils -45 mils) (rotation 0]) (justify "UpperRight") (textStyleRef "Default"))
		))
		(line (pt 200 mils 0 mils) (pt 400 mils 100 mils) (width 6 mils))
		(line (pt 400 mils 100 mils) (pt 400 mils -100 mils) (width 6 mils))
		(line (pt 400 mils -100 mils) (pt 200 mils 0 mils) (width 6 mils))
		(line (pt 200 mils 80 mils) (pt 200 mils -80 mils) (width 6 mils))
		(line (pt 200 mils 80 mils) (pt 240 mils 100 mils) (width 6 mils))
		(line (pt 160 mils -100 mils) (pt 200 mils -80 mils) (width 6 mils))
		(attr "RefDes" "RefDes" (pt 400 mils 350 mils) (justify LowerLeft) (isVisible True) (textStyleRef "Default"))

	)
	(compDef "CMDZ3L3_TR" (originalName "CMDZ3L3_TR") (compHeader (numPins 2) (numParts 1) (refDesPrefix Z)
		)
		(compPin "1" (pinName "K") (partNum 1) (symPinNum 1) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "2" (pinName "A") (partNum 1) (symPinNum 2) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(attachedSymbol (partNum 1) (altType Normal) (symbolName "CMDZ3L3_TR"))
		(attachedPattern (patternNum 1) (patternName "SOD2512X110N")
			(numPads 2)
			(padPinMap
				(padNum 1) (compPinRef "1")
				(padNum 2) (compPinRef "2")
			)
		)
		(attr "Supplier_Name" "RS")
		(attr "RS Part Number" "")
		(attr "Manufacturer_Name" "Central Semiconductor")
		(attr "Manufacturer_Part_Number" "CMDZ3L3 TR")
		(attr "Allied_Number" "")
		(attr "Other Part Number" "")
		(attr "Description" "ZENER DIODE 250mW, 3.3V  SOD-323")
		(attr "Datasheet Link" "https://www.centralsemi.com/get_document.php?cmp=1&mergetype=pd&mergepath=pd&pdf_id=CMDZ5L1-36L.PDF")
		(attr "Height" "1.1 mm")
		(attr "3D Package" "")
	)

)
