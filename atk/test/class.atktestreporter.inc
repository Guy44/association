<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage test
   *
   * @copyright (c)2005 Ivo Jansch
   * @copyright (c)2005 Marcus Baker
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 5935 $
   * $Id: class.atktestreporter.inc 5935 2008-05-20 14:22:15Z boy $
   */

  /**
   * This class is an extension to the default HtmlReporter of the SimpleTest
   * framework. The difference is that this reporter also displays succesful
   * passes. (Thanks to Marcus of http://www.lastcraft.com for the example
   * code).
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @author Marcus Baker <marcus@lastcraft.com>
   * @package atk
   * @subpackage test
   *
   */
  class atkTestReporter extends HtmlReporter
  {
    function paintLine($message, $result)
    {
      $breadcrumb = $this->getTestList();
      array_shift($breadcrumb);

      $line =
        "<tr class=\"$result\">".
          "<td valign=\"top\" class=\"date\" nowrap=\"nowrap\" width=\"5%\">".date('Y-m-d H:i:s')."</td>".
          "<td valign=\"top\" class=\"method\" nowrap=\"nowrap\" width=\"5%\">".implode("-&gt;", $breadcrumb)."</td>".
          "<td valign=\"top\" class=\"message\">".htmlentities($message)."</td>".
          "<td valign=\"top\" class=\"result\" nowrap=\"nowrap\" width=\"5%\">[".strtoupper($result)."]</td>".
        "</tr>";

      echo $line;
      flush();
    }

    function paintPass($message)
    {
      parent::paintPass($message);
      if(isset($_REQUEST["hidepassed"])) return;
      $this->paintLine($message, "pass");
    }

    function paintFail($message)
    {
      SimpleScorer::paintFail($message);
      $this->paintLine($message, "fail");
    }

    function _getCss()
    {
      return '
        BODY { font-family: tahoma, verdana, arial, helvetica }
        TR.fail { background-color: red;  color: white; font-family: tahoma, verdana, arial, helvetica; }
        TR.pass { color: darkgreen; font-family: tahoma, verdana, arial, helvetica;}
        TD.result { font-weight: bold }';
    }

    function paintDebug()
    {
      global $g_error_msg;

      if (count($g_error_msg) > 0 && (atkconfig("display_errors") || atkconfig("debug") > 0))
      {
         echo '<br><div style="font-family: monospace; font-size: 11px; color: #FF0000" align="left">error: '.implode("<br>\nerror: ",$g_error_msg).'</div>';
      }

      if (atkconfig("debug") > 0)
      {
        echo atkDebugger::getInstance()->renderDebugAndErrorMessages();
      }

      flush();
    }

    function paintHeader($test_name)
    {
      parent::paintHeader($test_name);
      echo "<table border=\"1\" cellspacing=\"0\" cellpadding=\"5\" width=\"100%\">";
      echo "<thead><tr><th>Stamp</th><th>Method</th><th>Message</th><th>Result</th></tr></thead>";
      echo "<tbody>";
      flush();
    }

    function paintFooter($test_name)
    {
      echo "</tbody>";
      echo "</table>";
      flush();

      parent::paintFooter($test_name);

      $this->paintDebug();
    }
  }


  /**
   * Includes
   * @access private
   */
  require_once(atkconfig("atkroot")."atk/test/simpletest/reporter.php");

  /**
   * This class is an extension to the default HtmlReporter of the SimpleTest
   * framework. The difference is that this reporter also displays succesful
   * passes and returns an array with the tesresults.
   *
   * @author Rene vd Ouden <rene@ibuildings.nl>
   * @package atk
   * @subpackage test
   *
   */
  class ArrayHtmlReporter extends HtmlReporter
  {
     var $_results;
     function arrayHtmlReporter()
     {
       $this->_results = array();
     }

     function paintException($message)
     {
       SimpleReporter::paintException($message);
        print "<span class=\"fail\">Exception</span>: ";
        $breadcrumb = $this->getTestList();
        array_shift($breadcrumb);
        print implode(" -&gt; ", $breadcrumb);
        print " -&gt; " . $message . "<br />\n";
     }

     function paintPass($message)
     {
       SimpleReporter::paintPass($message);
      $breadcrumb = $this->getTestList();
      array_shift($breadcrumb);
      $test = implode("->", $breadcrumb);

      $result["time"] = date('Y-m-d H:i:s');
      $result["status"] = "Passed";
      $result["test"] = $test;
      $result["message"] = $message;
      $this->_results[] = $result;
      echo str_pad(nl2br("  [<span class=\"pass\">Passed</span>] ".$result['time']." | ".$result['test']." : ".$result['message'] ." \n"), 4096);
      flush();
     }

     function paintFail($message)
     {
       SimpleReporter::paintFail($message);
      $breadcrumb = $this->getTestList();
      array_shift($breadcrumb);
      $test = implode("->", $breadcrumb);

      $result["time"] = date('Y-m-d H:i:s');
      $result["status"] = "Failed";
      $result["test"] = $test;
      $result["message"] = $message;
      $this->_results[] = $result;
      echo str_pad(nl2br("  [<span class=\"fail\">Failed</span>] ".$result['time']." | ".$result['test']." : ".$result['message'] ." \n"), 4096);
      flush();
     }

     function _getCss()
      {
        return '.fail {color: red; font-family: courier; } .pass { color: green; font-family: courier;}';
      }

     function getStatus()
     {
       return $this->_results;
     }
  }

  /**
   * This class is an extension to the default SimpleReporter of the SimpleTest
   * framework. The difference is that this reporter also displays succesful
   * passes and returns an array with the tesresults.
   *
   * @author Rene vd Ouden <rene@ibuildings.nl>
   * @package atk
   * @subpackage test
   */
  class ArrayTextReporter extends SimpleReporter
  {
     var $_results;
     function arrayTextReporter()
     {
       $this->SimpleReporter();
       $this->_results = array();
     }

     function paintPass($message)
     {
       parent::paintPass($message);
      $breadcrumb = $this->getTestList();
      array_shift($breadcrumb);
      $test = implode("->", $breadcrumb);

      $result["time"] = date('Y-m-d H:i:s');
      $result["status"] = "Passed";
      $result["test"] = $test;
      $result["message"] = $message;
      $this->_results[] = $result;
      echo ("  [".$result['status']."] ".$result['time']." | ".$result['test']." : ".$result['message'] ." \n");
     }

     function paintFail($message)
     {
       parent::paintFail($message);
      $breadcrumb = $this->getTestList();
      array_shift($breadcrumb);
      $test = implode("->", $breadcrumb);

      $result["time"] = date('Y-m-d H:i:s');
      $result["status"] = "Failed";
      $result["test"] = $test;
      $result["message"] = $message;
      $this->_results[] = $result;
      echo ("  [".$result['status']."] ".$result['time']." | ".$result['test']." : ".$result['message'] ." \n");
     }

     function paintFooter($test_name)
     {
       echo $this->getTestCaseProgress() . "/" . $this->getTestCaseCount();
      echo " test cases complete:\n";
      echo $this->getPassCount() . " passes, ";
      echo $this->getFailCount() . " fails and ";
      echo $this->getExceptionCount() . " exceptions.";
     }

     function getStatus()
     {
       return $this->_results;
     }
  }

?>
