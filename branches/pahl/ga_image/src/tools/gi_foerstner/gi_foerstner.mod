<MODULE class=GaImage name=förstner
        tip="<p>Determine optimal feature points</p>"
	cmd="gi_foerstner @input@ @output@ @qlim@ @wlim@ @filtersize@ @nonmaximasize@">
  <INPUT name=input label="Input image: ">
  <OUTPUT name=output label="Output image: ">

  <TEXT name=qlim label="Error Ellipse Roundness: " cmd="-q @qlim@">
  <TEXT name=wlim label="Error Ellipse Size: " cmd="-w @wlim@">
  <TEXT name=filtersize label="Filtersize: " cmd="-f @filtersize@">
  <TEXT name=nonmaximasize label="Non-Maxima Boxsize: " cmd="-m @nonmaximasize@">
</MODULE>

