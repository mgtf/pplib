<?

////
// $Id: session.inc,v 1.7 2003/09/04 13:43:15 mgf Exp $
// !session class
// Ce fichier est soumis à la licence d'utilisation
// contenue dans le fichier LICENCE
//

class session extends _stdclass {

  function session() {
    $this->_stdclass();
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
