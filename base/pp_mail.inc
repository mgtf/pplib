<?

// $Id: pp_mail.inc,v 1.9 2002/07/15 10:13:26 tieum Exp $

class pp_mail extends pp_mime
{
  var $from;
  var $to;
  var $subject;
  var $priority;
  var $message;
  var $content_type;
  var $attachment_data;
  var $has_attachment;
  var $is_html;

  function pp_mail()
  {
    $this->pp_mime();
    $this->priority = 1;
    $this->message = '';
    $this->subject = _("no subject").'.';
    $this->content_type = '';
    $this->has_attachment = FALSE;
    $this->is_html = FALSE;
    $this->attachment_data = array();
  }

  function send()
  {
    if(! strlen($this->message) ||
       ! strlen($this->from) ||
       ! strlen($this->to))
      user_error("pp_mail::send : "._("Missing parameters"), E_USER_ERROR);
    $headers  = "From: ".$this->from."\n";
    $headers .= "X-Mailer: pplib v".PP_MAJOR_VERSION.'.'.PP_MINOR_VERSION."\n";
    $headers .= "X-Priority: ".$this->priority."\n";
    $headers .= "Return-Path: ".GENERAL_WEBMASTER_MAIL."\n";
    if($this->is_html || $this->has_attachment)
      $headers .= "MIME-Version: 1.0\n";
    if($this->is_html && ! $this->has_attachment)
      $headers .= "Content-Type: text/html; charset=iso-8859-1\n";
    if($this->has_attachment)
    {
      $boundary = '--pplib-'.md5(uniqid(rand()));
      $headers .= 
	"Content-Type: multipart/mixed;\n".
	"\tboundary=\"$boundary\";\n".
	"Content-Transfer-Encoding: 7bit";
      $boundary = "--$boundary";
      // rewrite message body :
      $this->message = 
	"$boundary\n".
	($this->is_html ? 
	 "Content-Type: text/html; charset=iso-8859-1\n" : 
	 "Content-Type: text/plain; charset=us-ascii\n"
	).
	"Content-Transfer-Encoding: 7bit\n\n".
	$this->message."\n";
      // add attachments : 
      while(list($name, $data) = each($this->attachment_data))
      {
	$this->message .= 
	  "\n$boundary\n".
	  "Content-Type: ".$data['type'].";\n".
	  "\tname=\"$name\"\n".
	  "Content-Transfer-Encoding: base64\n".
	  "Content-Disposition: inline;\n".
	  "\tfilename=\"$name\"\n\n".
	  $data['data'];
      }
      $this->message .= "$boundary--\n";
    }
    mail($this->to, $this->subject, $this->message, $headers);
  }

  function add_attachment($name, $data, $ext = '')
  {
    $this->has_attachment = TRUE;
    $this->attachment_data[$name] = array(
	'data' => chunk_split(base64_encode($data)), 
	'type' => $this->get_mime($ext));
  }
}

?>
