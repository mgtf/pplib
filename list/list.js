<!-- list.js -->
<script language="JavaScript1.2">
  function invertsel(n) {
    len = document[n].elements.length;
    var i=0;
    for( i=0; i<len; i++)
      if (document[n].elements[i].type=="checkbox")
	document[n].elements[i].checked = !document[n].elements[i].checked;
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
