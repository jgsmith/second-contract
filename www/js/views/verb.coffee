SC.Views.Verb = Backbone.View.extend
  initialize: ->
    this.template = _.template $('#verb-template').html()
    this.listenTo this.model, 'change', this.render

  events:
    'click .save': 'save'

  render: ->
    json = this.model.toJSON()
    this.$el.html this.template json
    if this.model.isNew()
      this.$el.find("h1").text("New Verb")
    # manage settings that are difficult to put into the templates
    if json.actor?.position?
      for pos in json.actor.position
        this.$el.find("input[type='checkbox'][name='actor.position'][value='#{pos}']").prop {checked: true}
    if json.actor?.sense?
      for pos in json.actor.sense
        this.$el.find("input[type='checkbox'][name='actor.sense'][value='#{pos}']").prop {checked: true}
    if json.actor?.act?
      for pos in json.actor.act
        this.$el.find("input[type='checkbox'][name='actor.action'][value='#{pos}']").prop {checked: true}

    for pos in ["direct", "indirect", "instrumental"]
      do (pos) =>
        posEl = this.$el.find("input[type='checkbox'][name='arguments'][value='#{pos}']")
        if json.arguments?[pos]?.requirements?
          for req in json.arguments[pos].requirements
            this.$el.find("input[type='checkbox'][name='arguments.#{pos}'][value='#{req}']").prop
              checked: true
        $(posEl).change ->
          if $(posEl).is(':checked')
            $(posEl).parent().parent().parent().find(".fieldset-body").fadeIn()
          else
            $(posEl).parent().parent().parent().find(".fieldset-body").fadeOut()

        if json.arguments?[pos]?.used
          posEl.prop
            checked: true
          later ->
            $(posEl).parent().parent().parent().find(".fieldset-body").fadeIn()
        else
          later ->
            $(posEl).parent().parent().parent().find(".fieldset-body").fadeOut()
    this.el

  save: (e) ->
    e.preventDefault()
    data =
      verbs: (this.$('input[name="verbs"]').val() + "").split(/\s*,\s*/)
      brief: this.$('input[name="brief"]').val()
      help: this.$('textarea[name="help"]').val()
      see_also: (this.$('input[name="see-also"]').val() + "").split(/\s*,\s*/)
      disabled: this.$('input[name="disabled"]').is(":checked")
      action: (this.$('textarea[name="action"]').val() + "").split(/\s*\n\s*/)
      environment:
        respiration: ($(e).val() for e in this.$('input[name="respiration"]') when $(e).is(':checked'))
        timing: ($(e).val() for e in this.$('input[name="calendar"]') when $(e).is(':checked'))
        terrain: ($(e).val() for e in this.$('input[name="terrain"]') when $(e).is(':checked'))
        weather: ($(e).val() for e in this.$('input[name="weather"]') when $(e).is(':checked'))
      arguments:
        direct:
          used: this.$('input[name="arguments"][value="direct"]').is(":checked")
          requirements: ($(e).val() for e in this.$('input[name="arguments.direct"]') when $(e).is(':checked'))
        indirect:
          used: this.$('input[name="arguments"][value="indirect"]').is(":checked")
          requirements: ($(e).val() for e in this.$('input[name="arguments.indirect"]') when $(e).is(':checked'))
        instrumental:
          used: this.$('input[name="arguments"][value="instrumental"]').is(":checked")
          requirements: ($(e).val() for e in this.$('input[name="arguments.instrumental"]') when $(e).is(':checked'))


    this.model.set data

    if this.model.isNew()
      SC.Collections.verbs.create this.model,
        success: => console.log "Saved!"
    else
      this.model.save()
 
    false
