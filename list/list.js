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
<script language="JavaScript">
function confirm_del()
{
  if (confirm('{confirm_mesg}'))
  {
    document.{id}.{id}_want_save.value = 1;
    document.{id}.submit();
  }
}
</script>
<!-- end list.js -->
