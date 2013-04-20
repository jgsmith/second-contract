class SC.Views.Thing extends Backbone.View
  initialize: ->
    @template = _.template $('#thing-template').html()
    @listenTo @model, 'change', @render

  events:
    "change .event-selection": "updateEditorContent"
    "click .save": "save"
    "change select[name='ur.domain']": "updateUrArea"
    "change select[name='ur.area']": "updateUrWard"
    "change select[name='ur.ward']": "updateUrItem"

  updateUrArea: ->
    # given the domain, we figure out which areas are available
    uael = @$el.find("select[name='ur.area']")
    domain_id = @$el.find("select[name='ur.domain']").val()
    if domain_id? and domain_id != ""
      domain = SC.Collections.domains.get(domain_id)
      if domain?
        uael.empty()
        domain.areas.each (area) ->
          uael.append("<option value='#{area.id}'>#{area.id}</option>")
        @$el.find("select[name='ur.ward']").empty()
        @$el.find("select[name='ur.item']").empty()
        if domain.areas.length == 1
          @updateUrWard()
    else
      @$el.find("select[name='ur.area']").empty()
      @$el.find("select[name='ur.ward']").empty()
      @$el.find("select[name='ur.item']").empty()

  updateUrWard: ->
    # given the area, we figure out which wards are available
    uwel = @$el.find("select[name='ur.ward']")
    domain_id = @$el.find("select[name='ur.domain']").val()
    domain = SC.Collections.domains.get(domain_id)
    if domain?
      area_id = @$el.find("select[name='ur.area']").val()
      area = domain.areas.get(area_id)
      if area?
        uwel.empty()
        area.wards.each (ward) ->
          uwel.append("<option value='#{ward.id}'>#{ward.id}</option>")
        @$el.find("select[name='ur.item']").empty()
        if area.wards.length == 1
          @updateUrItem()

  updateUrItem: ->

  render: ->
    json = @model.toJSON()
    @$el.html @template json
    # now add the event handler names to the event selection
    # we'll make the editor hold the first event
    if json.events?
      sel = @$el.find(".event-selection")
      for e of json.events
        sel.append("<option value='#{e}'>#{e}</option>");

    later => 
      udel = @$el.find("select[name='ur.domain']")
      udel.append("<option value=''>No Template</option>")
      SC.Collections.domains.each (domain) ->
        console.log domain
        udel.append("<option value='#{domain.id}'>#{domain.id}</option>")
      udel.val("Default")
      @updateUrArea()
      @updateUrWard()
      @renderEditor()
      eventVal = @$(".event-selection").val()
      @editor.setValue @model.events.get(eventVal)?.get("content")
      @editor.navigateTo(0,0)
      @editingEvent = eventVal
      
    @el

  save: (e) ->
    e.preventDefault()
    data =
      ident: {}

    el = $(@el)
    data.ident.name = el.find("input[name='ident.name']").val()
    data.ident.cap_name = el.find("input[name='ident.cap_name']").val()
    if @editingEvent?
      data.events = @model.get('events')
      data.events[@editingEvent] = @editor.getValue()
    # we need to gather all of the details as well
    @model.set data
    @model.save()

  updateEditorContent: ->
    eventVal = @$(".event-selection").val()
    if eventVal != @editingEvent
      events = @model.get('events')
      if @editingEvent?
        events[@editingEvent] = @editor.getValue()
        @model.set({ 'events': events })
      @editor.setValue events[eventVal]
      @editor.navigateTo(0,0)
      @editingEvent = eventVal

  renderEditor: ->
    if !@editor?
      @editor = ace.edit("thing-event-editor")
      @editor.setTheme("ace/theme/textmate")
      @editor.getSession().setMode("ace/mode/c_cpp")

$("#new-event-modal").find(".btn-primary").click (e) ->
  e.preventDefault()
  event_name = $("#new-event-modal").find("input[name='name']").val()
  view = SC.App.currentViews["#main"]
  thing = view?.model
  if thing?
    events = thing.get('events')
    if not events.hasOwnProperty(event_name)
      events[event_name] = ""
      thing.set({'events': events})
    view.$(".event-selection").append("<option value='#{event_name}'>#{event_name}</option>")
    view.$(".event-selection").val(event_name)
      
  $("#new-event-modal").modal('hide')
