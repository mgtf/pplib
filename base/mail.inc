<?

// $Id: mail.inc,v 1.2 2002/04/04 09:29:11 tieum Exp $

class pp_mail extends pp_class
{
  var $from;
  var $to;
  var $subject;
  var $is_html;
  var $priority;

  function pp_mail()
  {
    $this->pp_class();
    $this->priority = 1;
    $this->is_html = 0;
  }

  function send()
  {
    if(! strlen($this->message) ||
       ! strlen($this->from) ||
       ! strlen($this->to))
      user_error("pp_mail::send : "._("Missing parameters"), E_USER_ERROR);
    $domain_name = DOMAIN;
    $headers  = "From: ".$this->from."\n";
    $headers .= "X-Mailer: pplib v".PP_MAJOR_VERSION.'.'.PP_MINOR_VERSION."\n";
    $headers .= "X-Priority: ".$this->priority."\n";
    $headers .= "Return-Path: ".GENERAL_WEBMASTER_MAIL."\n";
    if($this->is_html)
      $headers .= "Content-Type: text/html; charset=iso-8859-1\n";
    mail($this->to, $this->subject, $this->message, $headers);
  }
}

?>
