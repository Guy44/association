<?php
  
  useattrib("atkattribute");
  
  include_once("achievotools.inc");

  
  class mailinglist extends atkNode
  {
    
    function mailinglist()
    {
      $this->atkNode("mailinglist");
      $this->add(new atkManyToOneRelation("id_titre","table.titre",AF_RELATION_AUTOLINK|AF_OBLIGATORY)); 
    }
/*
    function getFilterRecord()
    {
      static $s_record = NULL;
      global $g_sessionManager;

      if ($s_record==NULL)
      {
        $s_record = array();
        $s_record["coordinator"] = $g_sessionManager->pageVar("coordinator");
        $s_record["from"] = $g_sessionManager->pageVar("from");
        $s_record["to"] = $g_sessionManager->pageVar("to");

        // convert attribute html values to internal values
        $attrs = &$this->getFilterAttrs();
        foreach (array_keys($attrs) as $attribname)
        {
          $p_attrib = &$attrs[$attribname];
          $s_record[$attribname] = &$p_attrib->fetchValue($s_record);
        }


        $go = $g_sessionManager->pageVar("go");
        if ($go!=1)
        {
          // initial loading. We'll put the from/to fields to reasonable defaults.

          $s_record["from"] = dateUtil::str2arr(dateUtil::startOfWeek(date("Ymd")));

          // next month
          $next_month = mktime(12,0,0,date("m")+1, date("d"), date("Y"));
          $s_record["to"] = dateUtil::str2arr(dateUtil::endOfWeek(date("Ymd", $next_month)));
        }
      }
      return $s_record;
    }

    function &getFilterAttrs()
    {
      useattrib("employee.myemployeesattribute");

      $attrs["coordinator"] = &new myEmployeesAttribute("coordinator", "reports.projectstatus.any_user");

      $attrs["from"] = &new atkDateAttribute("from");
      $attrs["to"] = &new atkDateAttribute("to");

      return $attrs;
    }

    function getFilterBar()
    {
      $record = $this->getFilterRecord();

      $attrs = &$this->getFilterAttrs();

      $output = '<form action="dispatch.php" method="get">';
      $output.= session_form();
      $output.= '<input type="hidden" name="go" value="1">'; //trigger to prevent loading the first time

      if ($this->hasAnyUserPrivilege())
      {
        $output.= atktext("coordinator").": ".$attrs["coordinator"]->edit($record)." ";
      }

      $output.= "<br/>";

      $output.= atktext("showonlyphasesfrom", "reports")." ".$attrs["from"]->edit($record)." ";
      $output.= atktext("to")." ".$attrs["to"]->edit($record);

      $output.= ' <input type="submit" value="'.atktext("refresh").'">';
      $output.= '</form>';

      return $output;
    }
*/
    function action_ajout()
    {
      // If adminPage implemented, call it and store the content result.
      //$content = $this->getProjectStatus();
print "toto";
die();
      // Add the given content to the page
      
	  
	  
	  //$page = &$this->getPage();
      //$page->addContent($this->renderActionPage("admin", $content));
    }
/*
    function getProjectStatus()
    {
      // Get a reference to the sessionmanager
      global $g_sessionManager;

      $this->addStyle("style.css");

      // Try to get a reference to the User Interface object and raise an error if it fails
      $ui = &$this->getUi();

      $go = $g_sessionManager->pageVar("go");

      // Start composing the output by adding the filterbar as topmost item
      $output = $this->getFilterBar();

      $filterrecord = $this->getFilterRecord();

      // Add a newline (vertical spacing)
      $output.= '<br/>';

      // Only get the planning if the filter was set and submitted
      if ($go == 1)
      {
        $db = &$this->getDb();
        $strfrom = dateutil::arr2str($filterrecord["from"]);
        $strto = dateutil::arr2str($filterrecord["to"]);

        $empfilter = "";
        if ($filterrecord["coordinator"]!=NULL)
        {
          $empfilter = " AND p.coordinator=".$filterrecord["coordinator"]["id"];
        }
        else if (!$this->hasAnyUserPrivilege()) // only show your own employees
        {
          $user = getUser();
          $empfilter = " AND p.coordinator=".$user["id"];
        }

        $rows = $db->getrows("SELECT
                                p.name as project,
                                ph.name as phase,
                                ph.initial_planning,
                                ph.current_planning,
                                sum(h.time) as minutes
                              FROM
                                project p,
                                phase ph,
                                hours h
                              WHERE
                                p.id = ph.projectid
                                AND h.phaseid = ph.id
                                AND ph.startdate < '$strto'
                                AND ph.enddate > '$strfrom'
                                AND ph.status = 'active'
                                AND p.status = 'active'
                                $empfilter
                              GROUP BY
                                p.name,
                                ph.name,
                                ph.initial_planning,
                                ph.current_planning
                              ORDER BY
                                p.name, ph.name");

         $data[] = array(atktext("project"), atktext("phase"), atktext("initial_planning"), atktext("current_planning"), atktext("booked"));

         $total_time = 0;
         $total_current = 0;
         $total_initial = 0;

         $previousname="";

         for ($i=0, $_i=count($rows); $i<$_i; $i++)
         {
           $line = array();

           $name = $rows[$i]["project"];
           if ($name!=$previousname)
           {
             $line[] = $name;

             $previousname = $name;
           }
           else
           {
             $line[] = "";
           }

           $line[] = $rows[$i]["phase"];
           $line[] = time_format($rows[$i]["initial_planning"]);
           $line[] = time_format($rows[$i]["current_planning"]);

           $booked = time_format($rows[$i]["minutes"]);

           if ($rows[$i]["minutes"]>$rows[$i]["current_planning"])
           {
             $booked = '<span style="color: '.COLOR_ERROR.'">'.$booked.'</span>';
           }
           else if ($rows[$i]["minutes"]>($rows[$i]["current_planning"]*0.9))
           {
             $booked = '<span style="color: '.COLOR_WARNING.'">'.$booked.'</span>';
           }

           $line[] = "<b>".$booked."</b>";

           $total_time += $rows[$i]["minutes"];
           $total_current += $rows[$i]["current_planning"];
           $total_initial += $rows[$i]["initial_planning"];

           $data[] = $line;
         }

         $data[] = array(atktext("total"), "", time_format($total_initial), time_format($total_current), time_format($total_time));

         $tr = &atknew("atk.utils.atktablerenderer");
         $output.= $tr->render($data, TBL_DATA|TBL_FOOTER, "recordlist")."<br/>";

      }

      // Render a box around the generated output
      $boxedoutput = $ui->renderBox(array("title"=>$this->actionTitle('ratereport'), "content"=>$output));

      // Return the generated boxed output
      return $boxedoutput;
    }

    function hasAnyUserPrivilege()
    {
      return $this->allowed("any_user");
    }
 */
  }
 
?>