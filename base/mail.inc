<?

////
// $Id: mail.inc,v 1.21 2003/09/03 09:07:12 mgf Exp $
// !send complex email with attachments
// Ce fichier est soumis � la licence d'utilisation
// contenue dans le fichier LICENCE
//
// public methods : 
//
// send()
// add_attachment($name, $data, $ext = '')
// add_notification($mail) { $this->headers .= "Disposition-Notification-To: $mail\n"; }
// add_recipient($mail) { $this->to[] = $mail; }
// add_copy_to_recipient($mail) { $this->cc[] = $mail; }
// add_blink_copy_to_recipient($mail) { $this->bcc[] = $mail; }
//
// private methods : 
//
// construct_attachment()
// construct_recipient_list()
// construct_copy_to_recipient()
//
// consts :
//
// BOUNDARY_PREFIX
// DEFAULT_PRIORITY

define('BOUNDARY_PREFIX', '--'.LIBNAME.'-');
define('DEFAULT_PRIORITY', 3);

class mail extends mime
{
  var $from;
  var $to;
  var $cc;
  var $bcc;
  var $subject;
  var $priority;
  var $body;
  var $headers;
  var $content_type;
  var $attachment_data;
  var $has_attachment;
  var $is_html;

  function mail()
  {
    $this->mime();
    $this->priority = DEFAULT_PRIORITY;
    $this->to = '';
    $this->cc = '';
    $this->bcc = '';
    $this->body = '';
    $this->subject = _("no subject").'.';
    $this->headers = '';
    $this->content_type = '';
    $this->has_attachment = FALSE;
    $this->is_html = FALSE;
    $this->attachment_data = array();
  }

  ////
  // !public
  function check($mail)
  {
    // first, check syntax.
    if(
	! strlen($mail) ||
	! eregi("^[_a-z0-9-]+(\.[_a-z0-9-]+)*@[a-z0-9-]+(\.[a-z0-9-]+)*(\.[a-z]{2,3})$", $mail)
      )
      return FALSE;
    // then, check mail server
    // XXX - not finished yet.
    return TRUE;
  }

  ////
  // !public
  function send()
  {
    if(! strlen($this->body) ||
       ! strlen($this->from) ||
       ! strlen($this->to))
      user_error("mail::send : "._("Missing parameters"), E_USER_ERROR);
    $this->headers  = "From: ".$this->from."\n";
    $this->headers .= "X-Mailer: ".LIBNAME." v".MAJOR_VERSION.'.'.MINOR_VERSION."\n";
    $this->headers .= "X-Priority: ".$this->priority."\n";
    $this->headers .= "Return-Path: ".SERVER_ADMIN."\n";
    if($this->is_html || $this->has_attachment)
      $this->headers .= "MIME-Version: 1.0\n";
    if($this->is_html && ! $this->has_attachment)
      $this->headers .= "Content-Type: text/html; charset=iso-8859-1\n";
    if($this->has_attachment)
      $this->construct_attachment();
    $this->construct_recipient_list();
    $this->construct_copy_to_recipient();
    $this->construct_blink_copy_to_recipient();
    mail($this->to, $this->subject, $this->body, $this->headers);
  }

  ////
  // !public
  function add_attachment($name, $data, $ext = '')
  {
    $this->has_attachment = TRUE;
    $this->attachment_data[$name] = array(
	'data' => chunk_split(base64_encode($data)), 
	'type' => $this->get_mime($ext));
  }

  ////
  // !public
  function add_notification($mail) { 
    $this->headers .= "Disposition-Notification-To: $mail\n"; 
  }

  ////
  // !public
  function add_recipient($mail) { $this->to[] = $mail; }

  ////
  // !public
  function add_copy_to_recipient($mail) { $this->cc[] = $mail; }

  ////
  // !public
  function add_blink_copy_to_recipient($mail) { $this->bcc[] = $mail; }

  ////
  // !public
  function construct_recipient_list()
  {
    if(! is_array($this->to))
      return;
    $this->to = implode(', ', $this->to);
  }

  ////
  // !private
  function construct_copy_to_recipient()
  {
    if(! is_array($this->cc))
    {
      if(strlen($this->cc))
	$this->headers .= 'Cc: '.$this->cc."\n";
    }
    else
      $this->headers .= 'Cc: '.implode(', ', $this->cc);
  }

  ////
  // !private
  function construct_blink_copy_to_recipient()
  {
    if(! is_array($this->bcc))
    {
      if(strlen($this->bcc))
	$this->headers .= 'Bcc: '.$this->bcc."\n";
    }
    else
      $this->headers .= 'Bcc: '.implode(', ', $this->bcc);
  }
 
  ////
  // !private
  function construct_attachment()
  {
    $boundary = BOUNDARY_PREFIX.md5(uniqid(rand()));
    $this->headers .= 
      "Content-Type: multipart/mixed;\n".
      "\tboundary=\"$boundary\";\n".
      "Content-Transfer-Encoding: 7bit";
    $boundary = "--$boundary";
    // rewrite body :
    $this->body = 
      "$boundary\n".
      ($this->is_html ? 
       "Content-Type: text/html; charset=iso-8859-1\n" : 
       "Content-Type: text/plain; charset=us-ascii\n"
      ).
      "Content-Transfer-Encoding: 7bit\n\n".
      $this->body."\n";
    // append attachments to the body : 
    while(list($name, $data) = each($this->attachment_data))
    {
      $this->body .= 
	"\n$boundary\n".
	"Content-Type: ".$data['type'].";\n".
	"\tname=\"$name\"\n".
	"Content-Transfer-Encoding: base64\n".
	"Content-Disposition: inline;\n".
	"\tfilename=\"$name\"\n\n".
	$data['data'];
    }
    $this->body .= "$boundary--\n";
  }
}

?>
