# KlattGrid_test.praat
# djmw 20081208, 20090420, 20140113


printline ===========		KlattGrid test start

call test_get_add_remove_extract_replace_old_interface

call test_get_add_remove_replace

printline ===========		KlattGrid test end

procedure test_get_add_remove_replace
	numberOfOralFormants = 6
	numberOfNasalFormants = 1
	numberOfTrachealFormants = 1
	numberOfFricationFormants = 5
	numberOfNasalAntiFormants = 1
	numberOfTrachealAntiFormants = 1
	numberOfDeltaFormants = 1
	tmin = 0
	tmax = 1

	kg = Create KlattGrid... kg tmin tmax numberOfOralFormants numberOfNasalFormants numberOfNasalAntiFormants 
		... numberOfFricationFormants numberOfTrachealFormants numberOfTrachealAntiFormants numberOfDeltaFormants

	printline ===========		KlattGrid test_get_add_remove_extract_replace start
	for .itime to 20
		call setget_1 "pitch" 130
		call setget_1 "voicing amplitude" 90
		call setget_1 "flutter" 0.5
		call setget_1 "power1" 2
		call setget_1 "power2" 3
		call setget_1 "open phase" 0.5
		call setget_1 "collision phase" 0.035
		call setget_1 "double pulsing" 0.4
		call setget_1 "spectral tilt" 20
		call setget_1 "aspiration amplitude" 90
		call setget_1 "breathiness amplitude" 90

		call setget_1 "frication bypass" 20
		call setget_1 "frication amplitude" 50
	
		call setget_formants Oral
		call setget_formants Nasal
		call setget_formants Tracheal
		call setget_formants Delta
		call setget_formants Frication
		call setget_antiformants Nasal
		call setget_antiformants Tracheal

	endfor

	call test_deltaFormants

	removeObject (kg)

	printline ===========		KlattGrid test_get_add_remove_extract_replace succesful

endproc

procedure test_get_add_remove_extract_replace_old_interface
	numberOfOralFormants = 6
	numberOfNormalFormants = numberOfOralFormants
	numberOfNasalFormants = 1
	numberOfTrachealFormants = 1
	numberOfFricationFormants = 5
	numberOfNasalAntiFormants = 1
	numberOfTrachealAntiFormants = 1
	numberOfDeltaFormants = 1
	tmin = 0
	tmax = 1

	printline ===========		KlattGrid test_get_add_remove_extract_replace_old_interface start

	kg = Create KlattGrid... kg tmin tmax numberOfOralFormants numberOfNasalFormants numberOfNasalAntiFormants 
		... numberOfFricationFormants numberOfTrachealFormants numberOfTrachealAntiFormants numberOfDeltaFormants

	for .itime to 20
		call setget_1 "pitch" 130
		call setget_1 "voicing amplitude" 90
		call setget_1 "flutter" 0.5
		call setget_1 "power1" 2
		call setget_1 "power2" 3
		call setget_1 "open phase" 0.5
		call setget_1 "collision phase" 0.035
		call setget_1 "double pulsing" 0.4
		call setget_1 "spectral tilt" 20
		call setget_1 "aspiration amplitude" 90
		call setget_1 "breathiness amplitude" 90

		call setget_1 "frication bypass" 20
		call setget_1 "frication amplitude" 50
	
		call setget_formants_old Normal
		call setget_formants_old Nasal
		call setget_formants_old Tracheal
		call setget_formants_old Frication
		call setget_antiformants_old Nasal
		call setget_antiformants_old Tracheal

		call setget_deltaformants

	endfor

	call test_deltaFormants_old

	removeObject (kg)
	printline ===========		KlattGrid test_get_add_remove_extract_replace_old_interface succesful
endproc

procedure setget_1 .var$ .value
	selectObject (kg)
	.time = randomUniform (tmin,tmax)
	Add '.var$' point... .time .value
	.val = Get '.var$' at time... .time
	.vt = Extract '.var$' tier
	selectObject (kg)
	Remove '.var$' points between... tmin tmax
	.val1 = Get '.var$' at time... .time
	assert .val1 = undefined; Add '.var$' at time... '.time' '.value'
	
	plusObject (.vt)
	Replace '.var$' tier
	selectObject (kg)
	.val1 = Get '.var$' at time... .time
	assert .val1 =.val; Add '.var$' at time... '.time' '.value'
	removeObject (.vt)
endproc

procedure setget_2p .var$ .ifor .time .value
	selectObject (kg)
	Add delta '.var$' point... .ifor .time .value
	.val = Get delta '.var$' at time... .ifor .time
	assert .val = .value; Add delta '.var$' point... .ifor .time .value
	.fg = Extract delta formant grid
	selectObject (kg)
	Remove delta '.var$' points between... .ifor tmin tmax
	.val1 = Get delta '.var$' at time... .ifor .time
	assert .val1 = undefined; Get delta '.var$' at time... .ifor .time
	plusObject (.fg)
	Replace delta formant grid
	selectObject (kg)
	.val1 = Get delta '.var$' at time... .ifor .time
	assert .val1 =.val; Get delta '.var$' at time... .ifor .time
	removeObject (.fg)
endproc

procedure setget_3_old .var$ .choice$ .ifor .time .value
	selectObject (kg)
	Add '.var$' point... "'.choice$'" .ifor .time .value
	.val = Get '.var$' at time... "'.choice$'" .ifor .time
	assert .val = .value; Add '.var$' point... "'.choice$'" .ifor .time .value
	.fg = Extract formant grid... '.choice$'
	selectObject (kg)
	Remove '.var$' points between... "'.choice$'" .ifor tmin tmax
	.val1 = Get '.var$' at time... "'.choice$'" .ifor .time
	assert .val1 = undefined; Get '.var$' at time... "'.choice$'" .ifor .time
	plusObject (.fg)
	Replace formant grid... '.choice$'
	selectObject (kg)
	.val1 = Get '.var$' at time... "'.choice$'" .ifor .time
	assert .val1 =.val; Get '.var$' at time... "'.choice$'" .ifor .time
	removeObject (.fg)

endproc

procedure setget_3a_old .var$ .choice$ .ifor .time .value
	selectObject (kg)
	Add '.var$' point... "'.choice$'" .ifor .time .value
	.val = Get '.var$' at time... "'.choice$'" .ifor .time
	assert .val = .value; Add '.var$' point... "'.choice$'" .ifor .time .value
	.tier = Extract amplitude tier... "'.choice$'" .ifor
	selectObject (kg)
	Remove '.var$' points between... "'.choice$'" .ifor tmin tmax
	.val1 = Get '.var$' at time... "'.choice$'" .ifor .time
	assert .val1 = undefined; Get '.var$' at time... "'.choice$'" .ifor .time
	plusObject (.tier)
	Replace amplitude tier... "'.choice$'" .ifor
	selectObject (kg)
	.val1 = Get '.var$' at time... "'.choice$'" .ifor .time
	assert .val1 =.val; Get '.var$' at time... "'.choice$'" .ifor .time
	removeObject (.tier)
endproc

procedure setget_formants_old .type$
	for .i to numberOf'.type$'Formants
		.time = randomUniform (tmin, tmax)
		.f = (2*.i-1) * randomUniform (450, 550)
		.b = .f /10
		.a = randomUniform (70, 90)
		call setget_3_old "formant" "'.type$' formant" .i .time .f
		call setget_3_old "bandwidth" "'.type$' formant" .i .time .b
		call setget_3a_old "amplitude" "'.type$' formant" .i .time .a
	endfor
	selectObject (kg)
	Formula (frequencies)... "'.type$' formant" self + 100
	Formula (bandwidths)... "'.type$' formant" self * 2
endproc


procedure setget_3 .type$ .formant$ .var$ .ifor .time .value
	selectObject (kg)
	what$ = "'.type$' '.formant$' '.var$'"
	Remove 'what$' points... .ifor tmin tmax
	Add 'what$' point... .ifor .time .value
	.val = Get 'what$' at time... .ifor .time
	assert .val = .value;	'.val' '.value' (Get 'what$' at time... '.ifor' '.time')
	.fg = Extract '.type$' '.formant$' grid
	selectObject (kg)
	Remove 'what$' points... .ifor tmin tmax
	.val1 = Get 'what$' at time... .ifor .time
	assert .val1 = undefined;	'.val1' (Get 'what$' at time... '.ifor' '.time')
	plusObject (.fg)
	Replace '.type$' '.formant$' grid
	selectObject (kg)
	.val1 = Get 'what$' at time... .ifor .time
	if .var$ = "amplitude"
		.val = undefined
	endif
	assert .val1 =.val; '.val1' '.val' (Get 'what$' at time... '.ifor' '.time')
	removeObject (.fg)
endproc

procedure setget_formants .tYPE$
	.type$ = replace_regex$ (.tYPE$, "(^.)","\L\1", 1)
	for .i to numberOf'.tYPE$'Formants
		.time = randomUniform (tmin, tmax)
		.f = (2*.i-1) * randomUniform (450, 550)
		.b = .f /10
		.a = randomUniform (70, 90)
		call setget_3 '.type$' "formant" "frequency" .i .time .f
		call setget_3 '.type$' "formant" "bandwidth" .i .time .b
		if .tYPE$ <> "Delta"
			call setget_3 '.type$' "formant" "amplitude" .i .time .a
		endif
	endfor
	selectObject (kg)
	Formula ('.type$' formant frequencies)... self + 100
	Formula ('.type$' formant bandwidths)... self * 2
endproc

procedure setget_antiformants .tYPE$
	.type$ = replace_regex$ (.tYPE$, "(^.)","\L\1", 1)
	for .i to numberOf'.tYPE$'AntiFormants
		.time = randomUniform (tmin, tmax)
		.f = (2*.i-1) * randomUniform (450, 550)
		.b = .f /10
		call setget_3 '.type$' "antiformant" "frequency" .i .time .f
		call setget_3 '.type$' "antiformant" "bandwidth" .i .time .b
	endfor
	selectObject (kg)
	Formula ('.type$' antiformant frequencies)... self + 100
	Formula ('.type$' antiformant bandwidths)... self * 2
endproc


procedure setget_deltaformants
	for .i to numberOfDeltaFormants
		.time = randomUniform (tmin, tmax)
		.f = randomUniform (50, 60)
		.b = .f /10
		call setget_2p "formant" .i .time .f
		call setget_2p "bandwidth" .i .time .b
	endfor
endproc

procedure setget_antiformants_old .type$
	for .i to numberOf'.type$'AntiFormants
		.time = randomUniform (tmin, tmax)
		.f = (2*.i-1) * randomUniform (450, 550)
		.b = .f /10
		call setget_3_old "formant" "'.type$' antiformant" .i .time .f
		call setget_3_old "bandwidth" "'.type$' antiformant" .i .time .b
	endfor
endproc

procedure test_deltaFormants_old
	.kg = Create KlattGrid... kg 0 1 6 1 1 6 1 1 1
	Add pitch point... 0.5 100
	Add voicing amplitude point... 0.5 90
	Add bandwidth point... "Normal formant" 1 0.5 50
	Add delta formant point... 1 0.5 500
	Remove formant points between... "Normal formant" 1 0 2
	Add formant point... "Normal formant" 1 0 400
	Add formant point... "Normal formant" 1 1 600
	Add formant point... "Normal formant" 1 0.003553 400
	Add formant point... "Normal formant" 1 0.002 300
	Add formant point... "Normal formant" 1 0.0112 430
	.sound = To Sound
	selectObject (.kg)
	.fg = Extract formant grid (open phases)... 0.1
	removeObject (.sound, .kg, .fg)
endproc

procedure test_deltaFormants
	.kg = Create KlattGrid... kg 0 1 6 1 1 6 1 1 1
	Add pitch point... 0.5 100
	Add voicing amplitude point... 0.5 90
	Add oral formant bandwidth point... 1 0.5 50
	Add delta formant frequency point... 1 0.5 500
	Remove oral formant frequency points... 1 0 2
	Add oral formant frequency point... 1 0 400
	Add oral formant frequency point... 1 1 600
	Add oral formant frequency point... 1 0.003553 400
	Add oral formant frequency point... 1 0.002 300
	Add oral formant frequency point... 1 0.0112 430
	.sound = To Sound
	selectObject (.kg)
	.fg = Extract oral formant grid (open phases)... 0.1
	removeObject (.sound, .fg, .kg)
endproc


