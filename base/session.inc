<?
//
// $Id: session.inc,v 1.1 2002/01/15 15:22:12 tieum Exp $
// session class

class pp_session extends pp_class
{
  var $session_name;

  function pp_session()
  {
    $this->pp_class();
    // session_name($this->session_name);
    return;

  }

  function destroy()
  {
    session_destroy();
    $p = session_get_cookie_params();
    setcookie(session_name(), "", 0, $p["path"], $p["domain"]); 
    header("Location: ?");
  }
}

?>
