<?

////
// $Id: pp_session.inc,v 1.5 2003/07/03 15:33:24 mgf Exp $
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
    die();
  }
};

?>
