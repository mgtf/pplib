<?

////
// $Id: pp_session.inc,v 1.4 2002/10/28 14:01:57 tieum Exp $
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
