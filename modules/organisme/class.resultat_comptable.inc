<?php
atkimport("atk.assmetanode");
useattrib("development.atgomanytoonerelation");
useattrib("development.atgtextlibelleattribute");
useattrib("development.atgnumberexpressionattribute");
class resultat_comptable extends assMetaNode {
	var $table = "organisme_compte";
	function resultat_comptable() {

		//		$this->atkMetaNode();
		$this->atkNode("resultat_comptable" ,NF_READONLY | NF_NO_SEARCH| NF_NO_VIEW | NF_MRA);
		$this->setSecurityAlias("organisme.organisme");
		$this->add(new atkTabbedPane("Pane1", array (
				"G�n�ral" => array (
						"id",
						"code",
						"libelle",
						"solde_initial",

						"solde_5",
						"solde_4",
						"solde_3",
						"solde_2",
						"solde_1",
						"solde",
						"solde_courant"
						//,		"solde_courant"

				)

		), AF_NO_LABEL));
		$this->add(new atkAttribute("id", AF_PRIMARY | AF_SEARCHABLE|AF_HIDE|AF_FORCE_LOAD), NULL, 10);
		$this->add(new atkAttribute("id_organisme", AF_SEARCHABLE|AF_HIDE|AF_FORCE_LOAD), NULL, 10);
		$this->add(new atkAttribute("id_creation", AF_HIDE_LIST), NULL, 10);
		$this->add(new atkAttribute("id_modification", AF_HIDE_LIST), NULL, 10);
		$this->add(new atkAttribute("date_creation", AF_HIDE_LIST), NULL, 10);
		$this->add(new atkAttribute("date_modification", AF_HIDE_LIST), NULL, 10);
			
		$this->add(new atkAttribute("code", AF_SEARCHABLE), NULL, 10);
		$this->add(new atgTextLibelleAttribute("libelle", AF_SEARCHABLE), NULL, 20);
		$this->add(new atkAttribute("debit_initial", AF_SEARCHABLE|AF_HIDE|AF_FORCE_LOAD), NULL, 30);
		$this->add(new atkAttribute("credit_initial", AF_SEARCHABLE|AF_HIDE|AF_FORCE_LOAD), NULL, 40);

		$this->add(new atkDummyAttribute("solde_initial", "solde initial", AF_NOLABEL), NULL, 50);
		$this->add(new atkDummyAttribute("solde_5", "solde initial", AF_NOLABEL), NULL, 60);
		$this->add(new atkDummyAttribute("solde_4", "solde initial", AF_NOLABEL), NULL, 61);
		$this->add(new atkDummyAttribute("solde_3", "solde initial", AF_NOLABEL), NULL, 62);
		$this->add(new atkDummyAttribute("solde_2", "solde initial", AF_NOLABEL), NULL, 63);
		$this->add(new atkDummyAttribute("solde_1", "solde initial", AF_NOLABEL), NULL, 64);
		$this->add(new atkDummyAttribute("solde", "solde initial", AF_NOLABEL), NULL, 65);
		$this->add(new atkDummyAttribute("solde_courant", "solde initial", AF_NOLABEL), NULL, 66);
		//	$this->add(new atkDummyAttribute("solde_courant", "solde courant", AF_NOLABEL), NULL, 80);
		/*		$this->add(new atgNumberExpressionAttribute("solde_5", ' select sum(debit-credit) from organisme_ligne_ecriture_comptable l, organisme_ecriture_comptable e
				where l.id_compte = [table].id and l.id_ecriture=e.id and year(e.date_ecriture)=year( UTC_TIMESTAMP())-5
				', AF_TOTAL, AF_TOTAL), NULL, 50);
		$this->add(new atgNumberExpressionAttribute("solde_4", ' select sum(debit-credit) from organisme_ligne_ecriture_comptable l, organisme_ecriture_comptable e
				where l.id_compte = [table].id and l.id_ecriture=e.id and year(e.date_ecriture)=year( UTC_TIMESTAMP())-4
				', AF_TOTAL, AF_TOTAL), NULL, 51);
		$this->add(new atgNumberExpressionAttribute("solde_3", ' select sum(debit-credit) from organisme_ligne_ecriture_comptable l, organisme_ecriture_comptable e
				where l.id_compte = [table].id and l.id_ecriture=e.id and year(e.date_ecriture)=year( UTC_TIMESTAMP())-3
				', AF_TOTAL, AF_TOTAL), NULL, 52);

		$this->add(new atgNumberExpressionAttribute("solde_2", ' select sum(debit-credit) from organisme_ligne_ecriture_comptable l, organisme_ecriture_comptable e
		where l.id_compte = [table].id and l.id_ecriture=e.id and year(e.date_ecriture)=year( UTC_TIMESTAMP())-2
		', AF_TOTAL, AF_TOTAL), NULL, 61);
		$this->add(new atgNumberExpressionAttribute("solde_1", ' select sum(debit-credit) from organisme_ligne_ecriture_comptable l, organisme_ecriture_comptable e
		where l.id_compte = [table].id and l.id_ecriture=e.id and year(e.date_ecriture)=year( UTC_TIMESTAMP())-1
		', AF_TOTAL, AF_TOTAL), NULL, 81);
		$this->add(new atgNumberExpressionAttribute("solde", ' select sum(debit-credit) from organisme_ligne_ecriture_comptable l, organisme_ecriture_comptable e
		where l.id_compte = [table].id and l.id_ecriture=e.id and year(e.date_ecriture)=year( UTC_TIMESTAMP())
		', AF_TOTAL, AF_TOTAL), NULL, 101);
		*/

		include_once (atkconfig("atkroot") . "modules/include/actions.inc");
		$this->setOrder(" RPAD(code,8,'*')");
		$this->setTable("organisme_compte");
		$this->m_securityMap["exporter"] = "view";
		$this->m_securityMap["exporter_3_niveaux"] = "view";



		$an_exercice=$this->m_postvars["an_exercice_comptable"];
		if (!isset($an_exercice))
		{
			$ancours=gmdate('Y');
		}
		$this->getAttribute("solde_5")->setLabel("S-".($ancours-5));
		$this->getAttribute("solde_4")->setLabel("S-".($ancours-4));
		$this->getAttribute("solde_3")->setLabel("S-".($ancours-3));
		$this->getAttribute("solde_2")->setLabel("S-".($ancours-2));
		$this->getAttribute("solde_1")->setLabel("S-".($ancours-1));
		$this->getAttribute("solde")->setLabel("S-".($ancours));
		$this->getAttribute("solde_courant")->setLabel("Total");
		$this->add(new atkBoolAttribute("usuel",AF_HIDE|AF_FORCE_LOAD));

	}

	function solde_courant_display($record, $mode) {
		$id_compte = $record["id"];
		$id_organisme = $record["id_organisme"];
		$db = & atkGetDb();
		if (strlen($record["code"])<4 )
		{
			$code=$record["code"].'%';
			$data = $db->getrows(' select sum(debit-credit) as solde from organisme_ligne_ecriture_comptable l, organisme_ecriture_comptable e
					where l.id_compte IN ( select id from organisme_compte where code like "'.$code.'" and id_organisme='.$id_organisme.' ) and l.id_ecriture=e.id
					');
			$data1 = $db->getrows(' select sum(debit_initial-credit_initial) as solde from organisme_compte
					where id IN ( select id from organisme_compte where code like "'.$code.'" and id_organisme='.$id_organisme.' )
					');

			if (empty ($data[0]["solde"])) {
				$data[0]["solde"] = "";
			}
			if (strlen($record["code"])==1 )
			{
				$resultat=$this->nombre_format(($data[0]["solde"] + $data1[0]["solde"]),"b",2) ;
			}
			if (strlen($record["code"])==2)
			{
				$resultat=$this->nombre_format(($data[0]["solde"] + $data1[0]["solde"]),"bi",2);
			}
			if (strlen($record["code"])==3)
			{
				$resultat=$this->nombre_format(($data[0]["solde"] + $data1[0]["solde"]),"i",2);
			}
			$resultat_brut=($data[0]["solde"] + $data1[0]["solde"]);
		}
		else
		{
			$data = $db->getrows(" select sum(debit-credit) as solde from organisme_ligne_ecriture_comptable l, organisme_ecriture_comptable e
					where l.id_compte = $id_compte and l.id_ecriture=e.id ");
			if (empty ($data[0]["solde"])) {
				$data[0]["solde"] = 0;
			}
			$resultat=$this->nombre_format(($data[0]["solde"] + $record["debit_initial"] - $record["credit_initial"]),"b",2);
			$resultat_brut= ($data[0]["solde"] + $record["debit_initial"] - $record["credit_initial"]);
		}
		if ($mode=='exportation')
		{
			if ($resultat_brut=="" || $resultat_brut=="0")
			{
				return '0.00';
			}
			else
			{return $resultat_brut;
			}
		}
		else
		{return 	$resultat;
		}

	}
	function solde_initial_display($record, $mode) {
		
		return $this->calcul_solde_initial($record,$ordre,$mode);

	}
	function solde_5_display($record, $mode) {
		$ordre=5;
		return $this->calcul_solde($record,$ordre,$mode);
	}
	function solde_4_display($record, $mode) {
		$ordre=4;
		return $this->calcul_solde($record,$ordre,$mode);
	}
	function solde_3_display($record, $mode) {
		$ordre=3;
		return $this->calcul_solde($record,$ordre,$mode);
	}
	function solde_2_display($record, $mode) {
		$ordre=2;
		return $this->calcul_solde($record,$ordre,$mode);
	}
	function solde_1_display($record, $mode) {
		$ordre=1;
		return $this->calcul_solde($record,$ordre,$mode);
	}
	function solde_display($record, $mode) {
		$ordre=0;
		return $this->calcul_solde($record,$ordre,$mode);
	}

	function calcul_solde($record,$ordre,$mode)
	{
		$id_compte = $record["id"];
		$id_organisme = $record["id_organisme"];
		$db = & atkGetDb();
		if (strlen($record["code"])<4)
		{
			$code=$record["code"].'%';
			$data = $db->getrows(' select sum(debit-credit) as solde from organisme_ligne_ecriture_comptable l
					where l.id_compte IN ( select id from organisme_compte where code like "'.$code.'" and id_organisme='.$id_organisme.' ) 
					and year(l.date_operation)=year( UTC_TIMESTAMP())-'.$ordre.'
					');
			if (empty ($data[0]["solde"])) {
				$data[0]["solde"] = "";
			}
			if (strlen($record["code"])==1)
			{
				$resultat=$this->nombre_format($data[0]["solde"],"b",2);

			}
			if (strlen($record["code"])==2)
			{
				$resultat=$this->nombre_format($data[0]["solde"],"bi",2);
			}
			if (strlen($record["code"])==3)
			{
				$resultat=$this->nombre_format($data[0]["solde"],"i",2);
			}
			$href=href(dispatch_url("organisme.ligne_ecriture_comptable", "admin", array(
					"atkfilter" => " id_compte IN ( select id from organisme_compte where code like '$code'
					and id_organisme=$id_organisme )
					and year(date_operation)=year( UTC_TIMESTAMP())-$ordre
					")
					, "dispatch.php"), $resultat, SESSION_NESTED);
		}
		else
		{
			$data = $db->getrows(' select sum(debit-credit) as solde from organisme_ligne_ecriture_comptable l
					where l.id_compte = '.$id_compte.'  and year(l.date_operation)=year( UTC_TIMESTAMP())-'.$ordre.'
					');
			if (empty ($data[0]["solde"])) {
				$data[0]["solde"] = "";
			}
			$resultat=$this->nombre_format($data[0]["solde"],"",2);
			$href=href(dispatch_url("organisme.ligne_ecriture_comptable", "admin", array(
					"atkfilter" => " id_compte = $id_compte
					and year(date_operation)=year( UTC_TIMESTAMP())-$ordre
					")
					, "dispatch.php"), $resultat, SESSION_NESTED);
				
		}
		$resultat_brut=$data[0]["solde"];
		if ($mode=='exportation')
		{
			if ($resultat_brut=="" || $resultat_brut=="0")
			{
				return '0.00';
			}
			else
			{return $resultat_brut;
			}
		}
		else
		{return 	$href;
		}
	}
	
	function calcul_solde_initial($record,$ordre,$mode)
	{
		$id_compte = $record["id"];
		$id_organisme = $record["id_organisme"];
		$db = & atkGetDb();
		if (strlen($record["code"])<4)
		{
			$code=$record["code"].'%';
			$data = $db->getrows(' select sum(debit_initial-credit_initial) as solde_initial from organisme_compte oc
					where oc.id IN ( select id from organisme_compte where code like "'.$code.'" and id_organisme='.$id_organisme.' )
					');
			if (empty ($data[0]["solde_initial"])) {
				$data[0]["solde_initial"] = "";
			}
			if (strlen($record["code"])==1)
			{
				$resultat=$this->nombre_format($data[0]["solde_initial"],"b",2);
	
			}
			if (strlen($record["code"])==2)
			{
				$resultat=$this->nombre_format($data[0]["solde_initial"],"bi",2);
			}
			if (strlen($record["code"])==3)
			{
				$resultat=$this->nombre_format($data[0]["solde_initial"],"i",2);
			}
			$href=href(dispatch_url("organisme.compte", "admin", array(
					"atkfilter" => " id IN ( select id from organisme_compte where code like '$code'
					and id_organisme=$id_organisme )
					")
					, "dispatch.php"), $resultat, SESSION_NESTED);
		}
		else
		{
		$data = $db->getrows(' select sum(debit_initial-credit_initial) as solde_initial from organisme_compte oc 
					where oc.id= '.$id_compte.' 				');
			if (empty ($data[0]["solde_initial"])) {
						$data[0]["solde_initial"] = "";
						}
						$resultat=$this->nombre_format($data[0]["solde_initial"],"",2);
						$href=href(dispatch_url("organisme.compte", "admin", array(
								"atkfilter" => " id= $id_compte		")
								, "dispatch.php"), $resultat, SESSION_NESTED);
	
		}
		$resultat_brut=$data[0]["solde_initial"];
				if ($mode=='exportation')
				{
				if ($resultat_brut=="" || $resultat_brut=="0")
				{
				return '0.00';
				}
				else
				{return $resultat_brut;
				}
				}
				else
				{return 	$href;
				}
	}
	
	
	function action_exporter()
	{
		$tout=0;
		$this->action_exporter_base($tout);
	}
	function action_exporter_3_niveaux()
	{
		$tout=3;
		$this->action_exporter_base($tout);
	}

	function action_exporter_base($tout)
	{

		if (is_array($this->m_postvars["atkselector"])) {
			$select = $this->m_postvars["atkselector"];
		} else {
			$select[0] = $this->m_postvars["atkselector"];
		}
		//		$this->p($this->m_postvars["atkselector"]);
		foreach ($select as $id_variable) {
			$array_id = explode('=', $id_variable);
			$idcomptes .= $array_id[1] . ',';
		}

		$idcomptes = substr($idcomptes, 0, strlen($idcomptes) - 1);
		$idcomptes=str_replace("'","",$idcomptes);



		$db = & atkGetDb();
		$an_exercice=$this->m_postvars["an_exercice_comptable"];
		if (!isset($an_exercice))
		{
			$ancours=gmdate('Y');
		}
		$query = & $db->createQuery();
		$query->addTable("organisme_compte");
		$query->addField("id as id");
		$query->addField("id_organisme as id_organisme");
		$query->addField("code as code");
		$query->addField("libelle as libelle");
		$query->addField("debit_initial as debit_initial");
		$query->addField("credit_initial as credit_initial");
		$query->addCondition("id in ( " .$idcomptes . ")");

		$query->addOrderBy(" RPAD(code,8,'*') ");
		$sql = $query->buildSelect();
		//$this->p($sql);
		$stuff = $db->getrows($sql);
		$tbl = & atknew("atk.utils.atktablerenderer");
		$data = array ();
		$tbl->setRowClass(1, "superbold");
		$row = array ();
		$row[] = "Classe";
		$row[] = "#";
		$row[] = "Code";
		$row[] = "Libelle";
		$row[] = "D�bit initial";
		$row[] = "Cr�dit initial";
		$row[] ="Solde initial";
		$row[] =("Solde ".($ancours-5));
		$row[] =("Solde ".($ancours-4));
		$row[] =("Solde ".($ancours-3));
		$row[] =("Solde ".($ancours-2));
		$row[] =("Solde ".($ancours-1));
		$row[] =("Solde ".($ancours));
		$row[] ="Solde courant";
		$data[] = $row;
		$mode="exportation";
		$j=0;
			
		for ($i = 0; $i < count($stuff); $i++) {

			if ($tout!=0 && strlen($stuff[$i]["code"])>$tout)
			{

			}
			else
			{
				$row = array ();
				$row[] ="classe".strlen($stuff[$i]["code"]);
				$row[] =$i+1;
				$row[] = $stuff[$i]["code"];
				$row[] = $stuff[$i]["libelle"];
				$row[] =str_replace(".",",",$stuff[$i]["debit_initial"]);
				$row[] =str_replace(".",",", $stuff[$i]["credit_initial"]);
				$record=array();
				$record["id"]=$stuff[$i]["id"];
				$record["code"]=$stuff[$i]["code"];
				$record["id_organisme"]=$stuff[$i]["id_organisme"];
				$record["credit_initial"]=$stuff[$i]["credit_initial"];
				$record["dedit_initial"]=$stuff[$i]["dedit_initial"];
				$row[] =str_replace(".",",",$this->solde_initial_display($record,$mode));
				$row[] =str_replace(".",",",$this->solde_5_display($record,$mode));
				$row[] =str_replace(".",",",$this->solde_4_display($record,$mode));
				$row[] =str_replace(".",",",$this->solde_3_display($record,$mode));
				$row[] =str_replace(".",",",$this->solde_2_display($record,$mode));
				$row[] =str_replace(".",",",$this->solde_1_display($record,$mode));
				$row[] =str_replace(".",",",$this->solde_display($record,$mode));
				$row[] =str_replace(".",",",$this->solde_courant_display($record,$mode));
//				if ( $row[5]=='0,00' && $row[6]=='0,00' && $row[7]=='0,00' && $row[8]=='0,00' && $row[9]=='0,00' && $row[10]=='0,00' && $row[11]=='0,00' && strlen($stuff[$i]["code"])!=1)
				if (  $row[6]=='0,00' && $row[7]=='0,00' && $row[8]=='0,00' && $row[9]=='0,00' && $row[10]=='0,00' && $row[11]=='0,00' && $row[12]=='0,00' && strlen($stuff[$i]["code"])!=1)
				{

				}
				else
				{
					$j+=1;
					if (strlen($stuff[$i]["code"])==1)
					{
						$data[]= array ();

							
						$row[1]='<b>'.$j;
					}
					$data[] = $row;
				}
			}


		}
		// render table.
		$res = $tbl->render($data, TBL_CLASS_IN_FIRST_CELL, "recordlist");
		// h// h
		$page = & atknew("atk.ui.atkpage");
		$ui = & atkinstance("atk.ui.atkui");
		$output = & atkOutput :: getInstance();
		$page->register_style($ui->stylePath("recordlist.css"));
		//				$res .= '<br><div align="right"><a href="javascript:window.close();">' . atktext("close") . '</a></div>';
		$page->addContent($res);
		$output->output($page->render(atktext('app_title') . ' - ' . atktext('help'), true));
		if ($this->m_postvars["atkexport"] == 'o') {
			$filename = $this->m_postvars["id"] . "_comptes.html";
			Header("Content-Disposition: attachment; filename=$filename");
			Header("Content-Length: " . strlen($output));
			Header("Connection: close");
			Header("Content-Type: text/html; name=$filename");
		}
	}


	function getView() {
		global $g_sessionManager;
		$view = $g_sessionManager->stackVar("view");
		if ($view == "") {
			$view = "active";
		}
		return $view;
	}

	function dispatch($postvars, $fullpage=true)
	{

		$id_organisme= $postvars['organisme_id'];

		$this->addFilter("organisme_compte.id_organisme=$id_organisme and (  organisme_compte.usuel='1'
				or organisme_compte.id in (select distinct id_compte from organisme_ligne_ecriture_comptable))");

		return parent::dispatch($postvars, $fullpage);
	}


	function code_display($record, $mode) {
		return str_pad($record["code"], 7, '.', STR_PAD_RIGHT);
	}

	function action_admin($handler) {
		global $config_recordsperpage;
		$config_recordsperpage = 1000;
		$view = $this->getView();
		global $ATK_VARS;

		$id_organisme= $ATK_VARS['organisme_id'];
		$classe_id=$ATK_VARS['classe_id'];
		if ($classe_id=="Tout")
		{$classe_id_like= " LEFT(organisme_compte.code,1)>'4' ";
		}
		else {
		$classe_id_like= " LEFT(organisme_compte.code,1)=$classe_id ";
		}

		if ($view == "active") {
				
			$this->addFilter("organisme_compte.id_organisme=$id_organisme and $classe_id_like  and (  organisme_compte.usuel='1'
					or organisme_compte.id in (select distinct id_compte from organisme_ligne_ecriture_comptable))");
				
				
				
		}
		return $handler->action_admin();
	}

	function rowColor($record) {
		$usuel = $record["usuel"];
		if (strlen($record["code"]) == 1 && $usuel == "1")
			return $this->couleurcompte1;
		if (strlen($record["code"]) == 2 && $usuel == "1")
			return $this->couleurcompte2;
		if (strlen($record["code"]) == 3 && $usuel == "1")
			return $this->couleurcompte3;
		if (strlen($record["code"]) > 3 && $usuel == "1")
			return $this->couleurcompte4;
	}

	function pageBreak($record) {
		if (strlen($record["code"]) == 1 )
		{
		return " page-break-before:always" ;}
	}
	
	function adminFooter() {
		$retour=parent::adminfooter();
		$view = $this->getView();
		$legende = 'L�gende des couleurs : ';
		$legende .= '<font color="' . $this->couleurcompte1 . '"> <b> Classe ' .		'<font color="' . $this->couleurcompte2 . '"> <b> Sous Classe ' .		'<font color="' . $this->couleurcompte3 . '"> <b> Compte 3 chiffres' .		'<font color="' . $this->couleurcompte4 . '"> <b> compte >= 4 chiffres ' .		'<font color="Black"> <b> compte non usuel ';
		;
		$legende .= '<br /> Classes : 1=capitaux, 2=immobilisations, 3=stocks, 4=tiers, 5=financiers, 6=charges, 7=produits, 8=contributions volontaires ';
		if ($view == "active") {
			return atktext("Consultation des comptes usuels.") . " " . href('dispatch.php?atknodetype=' . $this->atkNodeType() . '&atkaction=' . $this->m_action . '&view=nonactive', atktext('Cliquer ici pour visualiser tous les comptes.', $this->m_module, $this->m_type)) . "<br>" .			$legende . $retour;
		} else {
			return atktext("Consultation de tous les comptes.") . " " . href('dispatch.php?atknodetype=' . $this->atkNodeType() . '&atkaction=' . $this->m_action . '&view=active', atktext('Cliquer ici pour ne visualiser que les comptes usuels.', $this->m_module, $this->m_type)) . "<br>" .			$legende .$retour;
		}
	}
	function recordActions($rec, & $actions, & $mraactions) {
		//		parent::recordActions($rec, $actions, $mraactions);
		$mraactions['exporter'] = "exporter";
		$mraactions['exporter_3_niveaux'] = "exporter_3_niveaux";



	}


	function descriptor_def() {
		return "[code] - [libelle]";
	}
	function id_organisme_display($record, $mode) {
		(!empty($record["id_organisme"])) ? $retour=$this->descripteur_organisme($record["id_organisme"]["id"],'n') : $retour="";
		return $retour;
	}

}
?>