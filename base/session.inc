<?

////
// $Id: session.inc,v 1.3 2002/09/18 11:58:45 tieum Exp $
// !session class
// Ce fichier est soumis à la licence d'utilisation
// contenue dans le fichier LICENCE
//

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
};

?>
