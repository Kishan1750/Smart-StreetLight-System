function PostAction(action, myData, success)
{
	myData.action=action;
	$.ajax({
		url:"actions.php",
		type:"post",
		data:myData,
		success: success
	});
}
function ShowModal(title, content)
{
	//if title==Error, show error colors, else green
	$("#divModalTitle").html(title);
	$("#divModalBody").html(content);
	$("#divModal").modal('show');	
	var obj=$("#divModal .modal-dialog");
	if(window.innerHeight-$(obj).height()>30)
		$(obj).css('margin-top',parseInt((window.innerHeight-$(obj).height())/2)+'px');
	else
		$(obj).css('margin-top','10px');
	

}

function Loader()
{
	return '<span class="fa fa-spinner fa-spin"></span>';
}

$(document).ready(function(e) {
    $("body").append('<div id="divModal" class="modal myswal-show" role="dialog">\
  <div class="modal-dialog">\
\
    <!-- Modal content-->\
    <div class="modal-content">\
      <div class="modal-header">\
        <button type="button" class="close" data-dismiss="modal">&times;</button>\
        <h2 class="modal-title" id="divModalTitle" style="text-align:center">Modal Header</h4>\
      </div>\
      <div class="modal-body" id="divModalBody">\
        <p>Some text in the modal.</p>\
      </div>\
      <div class="modal-footer">\
        <button type="button" class="btn btn-default" data-dismiss="modal">Close</button>\
      </div>\
    </div>\
\
  </div>\
</div>');

$(".form-control").on('keyup change',function(){
	$(this).siblings('.error').remove();
});
});//document.ready