<?

////
// $Id: session.inc,v 1.6 2003/08/28 15:22:44 mgf Exp $
// !session class
// Ce fichier est soumis à la licence d'utilisation
// contenue dans le fichier LICENCE
//

class session extends _stdclass {
  var $session_name;

  function session() {
    $this->_stdclass();
    return;
  }

  function destroy() {
    session_destroy();
    $p = session_get_cookie_params();
    setcookie(session_name(), "", 0, $p["path"], $p["domain"]); 
    header("Location: ?");
    die();
  }
};

?>
