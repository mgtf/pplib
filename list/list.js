<!-- list.js -->
<script language="JavaScript1.2">
  function invertsel(n) {
    list = 'list' + n;
    len = document[list].elements.length;
    var i=0;
    for( i=0; i<len; i++)
      if (document[list].elements[i].type=="checkbox")
	document[list].elements[i].checked = !document[list].elements[i].checked;
  }
</script>
<!-- end list.js -->
