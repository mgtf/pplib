<!-- list.js -->
<script language="JavaScript1.2">
  function {id}_invertsel(name) {
    len = document[name].elements.length;
    var i=0;
    for( i=0; i<len; i++)
      if (document[name].elements[i].type=="checkbox")
	document[name].elements[i].checked = !document[name].elements[i].checked;
  }
</script>
<script language="JavaScript">
function {id}_confirm_del()
{
  if (confirm('{confirm_mesg}'))
  {
    document.{id}.{id}_want_save.value = 1;
    document.{id}.submit();
  }
}
</script>
<!-- end list.js -->
